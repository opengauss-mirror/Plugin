//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CAutoOptCtxt.h
//
//	@doc:
//		Optimizer context object; contains all global objects pertaining to
//		one optimization
//---------------------------------------------------------------------------
#ifndef SPQOPT_CAutoOptCtxt_H
#define SPQOPT_CAutoOptCtxt_H

#include "spqos/base.h"

#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"

namespace spqopt
{
using namespace spqos;

// forward declaration
class CCostParams;
class ICostModel;
class COptimizerConfig;
class IConstExprEvaluator;

//---------------------------------------------------------------------------
//	@class:
//		CAutoOptCtxt
//
//	@doc:
//		Auto optimizer context object creates and installs optimizer context
//		for unittesting
//
//---------------------------------------------------------------------------
class CAutoOptCtxt
{
private:
	// private copy ctor
	CAutoOptCtxt(CAutoOptCtxt &);

public:
	// ctor
	CAutoOptCtxt(CMemoryPool *mp, CMDAccessor *md_accessor,
				 IConstExprEvaluator *pceeval,
				 COptimizerConfig *optimizer_config);

	// ctor
	CAutoOptCtxt(CMemoryPool *mp, CMDAccessor *md_accessor,
				 IConstExprEvaluator *pceeval, ICostModel *pcm);

	// dtor
	~CAutoOptCtxt();

};	// class CAutoOptCtxt
}  // namespace spqopt


#endif	// !SPQOPT_CAutoOptCtxt_H

// EOF
