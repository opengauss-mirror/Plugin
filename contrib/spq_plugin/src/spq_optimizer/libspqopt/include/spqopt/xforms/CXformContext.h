//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformContext.h
//
//	@doc:
//		Context container passed to every application of a transformation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformContext_H
#define SPQOPT_CXformContext_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/operators/CPatternTree.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformContext
//
//	@doc:
//		context container
//
//---------------------------------------------------------------------------
class CXformContext : public CRefCount
{
private:
	// Memory pool
	CMemoryPool *m_mp;

	// private copy ctor
	CXformContext(const CXformContext &);

public:
	// ctor
	explicit CXformContext(CMemoryPool *mp) : m_mp(mp)
	{
	}

	// dtor
	~CXformContext()
	{
	}


	// accessor
	inline CMemoryPool *
	Pmp() const
	{
		return m_mp;
	}

};	// class CXformContext

}  // namespace spqopt


#endif	// !SPQOPT_CXformContext_H

// EOF
