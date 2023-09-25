//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CDistributionSpecStrictRandom_H
#define SPQOPT_CDistributionSpecStrictRandom_H

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecRandom.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDistributionSpecStrictRandom
//
//	@doc:
//		Class for representing forced random distribution.
//
//---------------------------------------------------------------------------
class CDistributionSpecStrictRandom : public CDistributionSpecRandom
{
public:
	//ctor
	CDistributionSpecStrictRandom();

	// accessor
	virtual EDistributionType
	Edt() const
	{
		return CDistributionSpec::EdtStrictRandom;
	}

	virtual const CHAR *
	SzId() const
	{
		return "STRICT RANDOM";
	}

	// does this distribution match the given one
	virtual BOOL Matches(const CDistributionSpec *pds) const;

	// does this distribution satisfy the given one
	virtual BOOL FSatisfies(const CDistributionSpec *pds) const;
};	// class CDistributionSpecStrictRandom
}  // namespace spqopt

#endif	// !SPQOPT_CDistributionSpecStrictRandom_H

// EOF
