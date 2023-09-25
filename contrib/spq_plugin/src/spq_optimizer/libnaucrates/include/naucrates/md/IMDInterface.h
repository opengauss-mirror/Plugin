//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		IMDInterface.h
//
//	@doc:
//		Base interface for metadata-related objects
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDInterface_H
#define SPQMD_IMDInterface_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

namespace spqmd
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		IMDInterface
//
//	@doc:
//		Base interface for metadata-related objects
//
//---------------------------------------------------------------------------
class IMDInterface : public CRefCount
{
public:
	virtual ~IMDInterface()
	{
	}
};
}  // namespace spqmd



#endif	// !SPQMD_IMDInterface_H

// EOF
