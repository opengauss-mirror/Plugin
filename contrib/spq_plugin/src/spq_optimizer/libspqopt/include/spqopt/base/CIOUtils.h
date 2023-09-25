//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CIOUtils.h
//
//	@doc:
//		Optimizer I/O utility functions
//---------------------------------------------------------------------------
#ifndef SPQOPT_CIOUtils_H
#define SPQOPT_CIOUtils_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CIOUtils
//
//	@doc:
//		Optimizer I/O utility functions
//
//---------------------------------------------------------------------------
class CIOUtils
{
public:
	// dump given string to output file
	static void Dump(CHAR *file_name, CHAR *sz);

};	// class CIOUtils
}  // namespace spqopt


#endif	// !SPQOPT_CIOUtils_H

// EOF
