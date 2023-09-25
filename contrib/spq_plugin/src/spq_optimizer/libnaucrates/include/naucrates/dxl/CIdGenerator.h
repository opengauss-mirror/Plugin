//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CIdGenerator.h
//
//	@doc:
//		Class providing methods for a ULONG counter
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CIdGenerator_H
#define SPQDXL_CIdGenerator_H

#define SPQDXL_INVALID_ID spqos::ulong_max

#include "spqos/base.h"

namespace spqdxl
{
using namespace spqos;

class CIdGenerator
{
private:
	ULONG id;

public:
	explicit CIdGenerator(ULONG);
	ULONG next_id();
	ULONG current_id();
};
}  // namespace spqdxl
#endif	// SPQDXL_CIdGenerator_H

// EOF
