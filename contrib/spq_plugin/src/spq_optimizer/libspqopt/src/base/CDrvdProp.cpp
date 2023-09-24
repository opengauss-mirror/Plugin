//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CDrvdProp.cpp
//
//	@doc:
//		Implementation of derived properties
//---------------------------------------------------------------------------

#include "spqopt/base/CDrvdProp.h"

#include "spqos/base.h"

#include "spqopt/operators/COperator.h"

#ifdef SPQOS_DEBUG
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/COptCtxt.h"
#endif	// SPQOS_DEBUG

FORCE_GENERATE_DBGSTR(spqopt::CDrvdProp);

namespace spqopt
{
CDrvdProp::CDrvdProp()
{
}

IOstream &
operator<<(IOstream &os, const CDrvdProp &drvdprop)
{
	return drvdprop.OsPrint(os);
}

}  // namespace spqopt

// EOF
