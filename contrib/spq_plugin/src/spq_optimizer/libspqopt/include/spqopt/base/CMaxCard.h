//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2012 EMC Corp.
//
//	@filename:
//		CMaxCard.h
//
//	@doc:
//		Data type to carry maximal card information;
//
//		This numeric is NOT an estimate but a guaranteed upper bound
//		on the cardinality, ie., plans can be pruned based on this information
//---------------------------------------------------------------------------
#ifndef SPQOPT_CMaxCard_H
#define SPQOPT_CMaxCard_H

#include "spqos/base.h"

// use spqos::ulong_max instead of spqos::ullong_max to make sure arithmetic does not cause overflows
#define SPQOPT_MAX_CARD ((ULLONG)(spqos::ulong_max))

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMaxCard
//
//	@doc:
//		Maximum Cardinality, including basic operators
//
//---------------------------------------------------------------------------
class CMaxCard
{
	// friends that enforce 'literal == variable' order
	friend BOOL operator==(ULLONG, const CMaxCard &);
	friend BOOL operator==(const CMaxCard &, const CMaxCard &);

private:
	// actual (cropped) value
	ULLONG m_ull;

	// equality
	BOOL
	operator==(ULLONG ull) const
	{
		SPQOS_ASSERT(ull <= SPQOPT_MAX_CARD);
		return m_ull == ull;
	}

public:
	// ctor
	explicit CMaxCard(ULLONG ull = SPQOPT_MAX_CARD) : m_ull(ull)
	{
		SPQOS_ASSERT(m_ull <= SPQOPT_MAX_CARD);
		m_ull = std::min(m_ull, SPQOPT_MAX_CARD);
	}

	// copy ctor
	CMaxCard(const CMaxCard &mc)
	{
		*this = mc;
	}

	// accessor
	ULLONG
	Ull() const
	{
		return m_ull;
	}

	// assignment
	void
	operator=(const CMaxCard &mc)
	{
		m_ull = mc.m_ull;
	}

	// arithmetic operators
	void
	operator*=(const CMaxCard &mc)
	{
		m_ull = std::min(mc.m_ull * m_ull, SPQOPT_MAX_CARD);
	}

	// arithmetic operators
	void
	operator+=(const CMaxCard &mc)
	{
		m_ull = std::min(mc.m_ull + m_ull, SPQOPT_MAX_CARD);
	}

	// print
	IOstream &
	OsPrint(IOstream &os) const
	{
		if (SPQOPT_MAX_CARD == m_ull)
		{
			return os << "unbounded";
		}

		return os << m_ull;
	}
};	// class CMaxCard


// shorthand for printing
inline IOstream &
operator<<(IOstream &os, const CMaxCard &mc)
{
	return mc.OsPrint(os);
}

// shorthand for less-than equal
inline BOOL
operator<=(const CMaxCard &mcLHS, const CMaxCard &mcRHS)
{
	if (mcLHS.Ull() <= mcRHS.Ull())
	{
		return true;
	}

	return false;
}

// shorthand for equality
inline BOOL
operator==(const CMaxCard &mcLHS, const CMaxCard &mcRHS)
{
	return mcLHS.operator==(mcRHS.Ull());
}

// shorthand for equality
inline BOOL
operator==(ULLONG ull, const CMaxCard &mc)
{
	return mc.operator==(ull);
}

}  // namespace spqopt

#endif	// !SPQOPT_CMaxCard_H

// EOF
