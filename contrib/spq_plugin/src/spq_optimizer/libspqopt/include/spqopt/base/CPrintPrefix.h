//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPrintPrefix.h
//
//	@doc:
//		Implemenation of prefix sequence for debug print functions;
//		In the common use case the actual strings are constants and a chain
//		of prefixes is built up on the stack;
//		Not ref-counted nor does it own any of the data it points to.
//---------------------------------------------------------------------------
#ifndef SPQOS_CPrintPrefix_H
#define SPQOS_CPrintPrefix_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CPrintPrefix
//
//	@doc:
//		Prefix element for debug print functions
//
//---------------------------------------------------------------------------
class CPrintPrefix
{
private:
	// previous prefix; usually in the previous stack frame
	const CPrintPrefix *m_ppfx;

	// actual string
	const CHAR *m_sz;

public:
	// ctor
	explicit CPrintPrefix(const CPrintPrefix *, const CHAR *);

	// print
	IOstream &OsPrint(IOstream &os) const;

};	// class CPrintPrefix


// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CPrintPrefix &pfx)
{
	return pfx.OsPrint(os);
}

}  // namespace spqopt

#endif	// !SPQOS_CPrintPrefix_H


// EOF
