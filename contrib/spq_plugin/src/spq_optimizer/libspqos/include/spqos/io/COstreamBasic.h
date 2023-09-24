//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		COstreamBasic.h
//
//	@doc:
//		Output stream base class;
//---------------------------------------------------------------------------
#ifndef SPQOS_COstreamBasic_H
#define SPQOS_COstreamBasic_H

#include "spqos/io/COstream.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		COstreamBasic
//
//	@doc:
//		Implements a basic write thru interface over a std::WOSTREAM
//
//---------------------------------------------------------------------------

class COstreamBasic : public COstream
{
private:
	// underlying stream
	WOSTREAM *m_ostream;

	// private copy ctor
	COstreamBasic(const COstreamBasic &);

public:
	// please see comments in COstream.h for an explanation
	using COstream::operator<<;

	// ctor
	explicit COstreamBasic(WOSTREAM *ostream);

	virtual ~COstreamBasic()
	{
	}

	// implement << operator
	virtual IOstream &operator<<(const WCHAR *);

	// implement << operator
	virtual IOstream &operator<<(const WCHAR);
};

}  // namespace spqos

#endif	// !SPQOS_COstreamBasic_H

// EOF
