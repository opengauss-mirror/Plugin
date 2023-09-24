//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPattern.h
//
//	@doc:
//		Base class for all pattern operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPattern_H
#define SPQOPT_CPattern_H

#include "spqos/base.h"

#include "spqopt/operators/COperator.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CPattern
//
//	@doc:
//		base class for all pattern operators
//
//---------------------------------------------------------------------------
class CPattern : public COperator
{
private:
	// private copy ctor
	CPattern(const CPattern &);

public:
	// ctor
	explicit CPattern(CMemoryPool *mp) : COperator(mp)
	{
	}

	// dtor
	virtual ~CPattern()
	{
	}

	// type of operator
	virtual BOOL
	FPattern() const
	{
		SPQOS_ASSERT(!FPhysical() && !FScalar() && !FLogical());
		return true;
	}

	// create derived properties container
	virtual CDrvdProp *PdpCreate(CMemoryPool *mp) const;

	// create required properties container
	virtual CReqdProp *PrpCreate(CMemoryPool *mp) const;

	// match function
	BOOL Matches(COperator *) const;

	// sensitivity to order of inputs
	BOOL FInputOrderSensitive() const;

	// check if operator is a pattern leaf
	virtual BOOL FLeaf() const = 0;

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// conversion function
	static CPattern *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(pop->FPattern());

		return reinterpret_cast<CPattern *>(pop);
	}

	// helper to check multi-node pattern
	static BOOL
	FMultiNode(COperator *pop)
	{
		return COperator::EopPatternMultiLeaf == pop->Eopid() ||
			   COperator::EopPatternMultiTree == pop->Eopid();
	}

};	// class CPattern

}  // namespace spqopt


#endif	// !SPQOPT_CPattern_H

// EOF
