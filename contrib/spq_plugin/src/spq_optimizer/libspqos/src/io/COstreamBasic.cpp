//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2010 Greenplum, Inc.
//
//	@filename:
//		COstreamBasic.cpp
//
//	@doc:
//		Implementation of basic wide character output stream
//---------------------------------------------------------------------------

#include "spqos/io/COstreamBasic.h"

#include "spqos/base.h"
#include "spqos/io/ioutils.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		COstreamBasic::COstreamBasic
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
COstreamBasic::COstreamBasic(WOSTREAM *pos) : COstream(), m_ostream(pos)
{
	SPQOS_ASSERT(NULL != m_ostream && "Output stream cannot be NULL");
}

//---------------------------------------------------------------------------
//	@function:
//		COstreamBasic::operator<<
//
//	@doc:
//		WCHAR write thru;
//
//---------------------------------------------------------------------------
IOstream &
COstreamBasic::operator<<(const WCHAR *wsz)
{
	m_ostream = &(*m_ostream << wsz);
	return *this;
}

//---------------------------------------------------------------------------
//	@function:
//		COstreamBasic::operator<<
//
//	@doc:
//		WCHAR write thru;
//
//---------------------------------------------------------------------------
IOstream &
COstreamBasic::operator<<(const WCHAR wc)
{
	m_ostream = &(*m_ostream << wc);
	return *this;
}

// EOF
