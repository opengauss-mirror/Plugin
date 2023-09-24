//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSetIter.h
//
//	@doc:
//		Implementation of iterator for column ref sets
//---------------------------------------------------------------------------
#ifndef SPQOS_CColRefSetIter_H
#define SPQOS_CColRefSetIter_H

#include "spqos/base.h"
#include "spqos/common/CBitSetIter.h"

#include "spqopt/base/CColRefSet.h"

namespace spqopt
{
// fwd declarations
class CColumnFactory;

//---------------------------------------------------------------------------
//	@class:
//		CColRefSetIter
//
//	@doc:
//		Iterator for colref set's; defined as friend, ie can access colrefset's
//		internal links
//
//---------------------------------------------------------------------------
class CColRefSetIter : public CBitSetIter
{
private:
	// a copy of the pointer to column factory, obtained at construction time
	CColumnFactory *m_pcf;

	// private copy ctor
	CColRefSetIter(const CColRefSetIter &);

	// current bit -- private to make super class' inaccessible
	ULONG UlBit() const;

public:
	// ctor
	explicit CColRefSetIter(const CColRefSet &bs);

	// dtor
	~CColRefSetIter()
	{
	}

	// current colref
	CColRef *Pcr() const;

};	// class CColRefSetIter
}  // namespace spqopt

#endif	// !SPQOS_CColRefSetIter_H


// EOF
