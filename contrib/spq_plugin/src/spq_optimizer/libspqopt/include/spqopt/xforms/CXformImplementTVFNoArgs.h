//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementTVFNoArgs.h
//
//	@doc:
//		Implement logical TVF with a physical TVF with no arguments
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementTVFNoArgs_H
#define SPQOPT_CXformImplementTVFNoArgs_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementTVF.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformImplementTVFNoArgs
//
//	@doc:
//		Implement TVF with no arguments
//
//---------------------------------------------------------------------------
class CXformImplementTVFNoArgs : public CXformImplementTVF
{
private:
	// private copy ctor
	CXformImplementTVFNoArgs(const CXformImplementTVFNoArgs &);

public:
	// ctor
	explicit CXformImplementTVFNoArgs(CMemoryPool *mp);

	// dtor
	virtual ~CXformImplementTVFNoArgs()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementTVFNoArgs;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementTVFNoArgs";
	}

};	// class CXformImplementTVFNoArgs

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementTVFNoArgs_H

// EOF
