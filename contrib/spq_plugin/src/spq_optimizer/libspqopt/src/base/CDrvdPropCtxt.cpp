//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Software, Inc.
//
//	@filename:
//		CDrvdPropCtxt.cpp
//
//	@doc:
//		Implementation of derived properties context
//---------------------------------------------------------------------------

#include "spqopt/base/CDrvdPropCtxt.h"

#include "spqos/base.h"

#ifdef SPQOS_DEBUG
#include "spqos/error/CAutoTrace.h"
#endif	// SPQOS_DEBUG

namespace spqopt
{
IOstream &
operator<<(IOstream &os, CDrvdPropCtxt &drvdpropctxt)
{
	return drvdpropctxt.OsPrint(os);
}

}  // namespace spqopt

// EOF
