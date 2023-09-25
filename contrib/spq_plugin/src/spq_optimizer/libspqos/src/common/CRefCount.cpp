//---------------------------------------------------------------------------
//	Greenplum Database
//  Copyright (c) 2020 VMware, Inc.
//
//	@filename:
//		CRefCount.cpp
//
//	@doc:
//		Implementation of class for ref-counted objects
//---------------------------------------------------------------------------

#include "spqos/common/CRefCount.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/task/CTask.h"

using namespace spqos;
