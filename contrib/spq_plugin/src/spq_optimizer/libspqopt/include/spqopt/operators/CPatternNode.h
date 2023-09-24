//---------------------------------------------------------------------------
// Greenplum Database
// Copyright (c) 2020 VMware and affiliates, Inc.
//
// CPatternNode.h
//
// A node that matches multiple operators, to be used in patterns of xforms.
// The exact matching algorithm depends on an enum that is passed in the
// constructor.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPatternNode_H
#define SPQOPT_CPatternNode_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/operators/CPattern.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
// CPatternNode
//
// An operator that can match multiple other operator types
//---------------------------------------------------------------------------
class CPatternNode : public CPattern
{
public:
	enum EMatchType
	{
		EmtMatchInnerOrLeftOuterJoin,
		EmtSentinel
	};

private:
	// private copy ctor
	CPatternNode(COperator &);

	enum EMatchType m_match;

public:
	// ctor
	explicit CPatternNode(CMemoryPool *mp, enum EMatchType matchType)
		: CPattern(mp), m_match(matchType)
	{
	}

	// dtor
	virtual ~CPatternNode()
	{
	}

	// match function
	BOOL
	Matches(COperator *pop) const
	{
		return Eopid() == pop->Eopid() &&
			   m_match == static_cast<CPatternNode *>(pop)->m_match;
	}

	// check if operator is a pattern leaf
	virtual BOOL
	FLeaf() const
	{
		return false;
	}

	// conversion function
	static CPatternNode *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(COperator::EopPatternNode == pop->Eopid());

		return static_cast<CPatternNode *>(pop);
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPatternNode;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPatternNode";
	}

	BOOL
	MatchesOperator(enum COperator::EOperatorId opid)
	{
		switch (m_match)
		{
			case EmtMatchInnerOrLeftOuterJoin:
				return COperator::EopLogicalInnerJoin == opid ||
					   COperator::EopLogicalLeftOuterJoin == opid;

			default:
				return false;
		}
	}

};	// class CPatternNode
}  // namespace spqopt


#endif	// !SPQOPT_CPatternNode_H

// EOF
