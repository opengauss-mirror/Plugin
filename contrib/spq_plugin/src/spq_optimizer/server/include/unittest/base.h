//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal Inc.
//
//	@filename:
//		base.h
//
//	@doc:
//		Global definitions for optimizer unittests
//---------------------------------------------------------------------------
#ifndef UNITTEST_BASE_H
#define UNITTEST_BASE_H

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "spqdbcost/CCostModelSPQDB.h"
#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "naucrates/md/CMDProviderMemory.h"

// number of segments for running tests
#define SPQOPT_TEST_SEGMENTS 2

using namespace spqopt;
using namespace spqmd;
using namespace spqdxl;
using namespace spqdbcost;

#endif	// UNITTEST_BASE_H

// EOF
