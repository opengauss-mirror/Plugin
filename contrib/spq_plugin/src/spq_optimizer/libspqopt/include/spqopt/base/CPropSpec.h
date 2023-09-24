//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPropSpec.h
//
//	@doc:
//		Abstraction for specification of properties;
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPropSpec_H
#define SPQOPT_CPropSpec_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;

// prototypes
class CReqdPropPlan;

//---------------------------------------------------------------------------
//	@class:
//		CPropSpec
//
//	@doc:
//		Property specification
//
//---------------------------------------------------------------------------
class CPropSpec : public CRefCount, public DbgPrintMixin<CPropSpec>
{
public:
	// property type
	enum EPropSpecType
	{
		EpstOrder,
		EpstDistribution,
		EpstRewindability,
		EpstPartPropagation,

		EpstSentinel
	};

private:
	// private copy ctor
	CPropSpec(const CPropSpec &);

protected:
	// ctor
	CPropSpec()
	{
	}

	// dtor
	~CPropSpec()
	{
	}

public:
	// append enforcers to dynamic array for the given plan properties
	virtual void AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CReqdPropPlan *prpp,
								 CExpressionArray *pdrspqexpr,
								 CExpression *pexpr) = 0;

	// hash function
	virtual ULONG HashValue() const = 0;

	// extract columns used by the property
	virtual CColRefSet *PcrsUsed(CMemoryPool *mp) const = 0;

	// property type
	virtual EPropSpecType Epst() const = 0;

};	// class CPropSpec


// shorthand for printing
inline IOstream &
operator<<(IOstream &os, const CPropSpec &ospec)
{
	return ospec.OsPrint(os);
}

}  // namespace spqopt

FORCE_GENERATE_DBGSTR(spqopt::CPropSpec);

#endif	// !SPQOPT_CPropSpec_H

// EOF
