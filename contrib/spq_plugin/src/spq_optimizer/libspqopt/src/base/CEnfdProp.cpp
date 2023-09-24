//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Software, Inc.
//
//	@filename:
//		CEnfdProp.cpp
//
//	@doc:
//		Implementation of enforced property
//---------------------------------------------------------------------------

#include "spqopt/base/CEnfdProp.h"

#include "spqos/base.h"

#ifdef SPQOS_DEBUG
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/COptCtxt.h"
#endif	// SPQOS_DEBUG

FORCE_GENERATE_DBGSTR(spqopt::CEnfdProp);

namespace spqopt
{
IOstream &
operator<<(IOstream &os, CEnfdProp &efdprop)
{
	return efdprop.OsPrint(os);
}

}  // namespace spqopt

// EOF
