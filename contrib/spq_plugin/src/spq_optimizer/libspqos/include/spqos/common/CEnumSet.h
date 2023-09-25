//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CEnumSet.h
//
//	@doc:
//		Implementation of set of enums as bitset
//---------------------------------------------------------------------------
#ifndef SPQOS_CEnumSet_H
#define SPQOS_CEnumSet_H

#include "spqos/base.h"
#include "spqos/common/CBitSet.h"


namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CEnumSet
//
//	@doc:
//		Template derived from CBitSet
//
//---------------------------------------------------------------------------
template <class T, ULONG sentinel_index>
class CEnumSet : public CBitSet
{
private:
	// hidden copy ctor
	CEnumSet<T, sentinel_index>(const CEnumSet<T, sentinel_index> &);

public:
	// ctor
	explicit CEnumSet<T, sentinel_index>(CMemoryPool *mp)
		: CBitSet(mp, sentinel_index)
	{
	}

	explicit CEnumSet<T, sentinel_index>(
		CMemoryPool *mp, const CEnumSet<T, sentinel_index> &enum_set)
		: CBitSet(mp, enum_set)
	{
	}

	// dtor
	virtual ~CEnumSet<T, sentinel_index>()
	{
	}

	// determine if bit is set
	BOOL
	Get(T t) const
	{
		SPQOS_ASSERT(t >= 0);

		ULONG bit_index = static_cast<ULONG>(t);
		SPQOS_ASSERT(bit_index < sentinel_index && "Out of range of enum");

		return CBitSet::Get(bit_index);
	}

	// set given bit; return previous value
	BOOL
	ExchangeSet(T t)
	{
		SPQOS_ASSERT(t >= 0);

		ULONG bit_index = static_cast<ULONG>(t);
		SPQOS_ASSERT(bit_index < sentinel_index && "Out of range of enum");

		return CBitSet::ExchangeSet(bit_index);
	}

	// clear given bit; return previous value
	BOOL
	ExchangeClear(T t)
	{
		SPQOS_ASSERT(t >= 0);

		ULONG bit_index = static_cast<ULONG>(t);
		SPQOS_ASSERT(bit_index < sentinel_index && "Out of range of enum");

		return CBitSet::ExchangeClear(bit_index);
	}

};	// class CEnumSet
}  // namespace spqos

#endif	// !SPQOS_CEnumSet_H

// EOF
