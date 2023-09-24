//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CBitSetIter.h
//
//	@doc:
//		Implementation of iterator for bitset
//---------------------------------------------------------------------------
#ifndef SPQOS_CBitSetIter_H
#define SPQOS_CBitSetIter_H

#include "spqos/base.h"
#include "spqos/common/CBitSet.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CBitSetIter
//
//	@doc:
//		Iterator for bitset's; defined as friend, ie can access bitset's
//		internal links
//
//---------------------------------------------------------------------------
class CBitSetIter
{
private:
	// bitset
	const CBitSet &m_bs;

	// current cursor position (in current link)
	ULONG m_cursor;

	// current cursor link
	CBitSet::CBitSetLink *m_bsl;

	// is iterator active or exhausted
	BOOL m_active;

	// private copy ctor
	CBitSetIter(const CBitSetIter &);

public:
	// ctor
	explicit CBitSetIter(const CBitSet &bs);
	// dtor
	~CBitSetIter()
	{
	}

	// short hand for cast
	operator BOOL() const
	{
		return m_active;
	}

	// move to next bit
	BOOL Advance();

	// current bit
	ULONG Bit() const;

};	// class CBitSetIter
}  // namespace spqos


#endif	// !SPQOS_CBitSetIter_H

// EOF
