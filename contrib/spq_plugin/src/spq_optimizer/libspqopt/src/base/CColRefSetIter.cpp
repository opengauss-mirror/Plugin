//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSetIter.cpp
//
//	@doc:
//		Implementation of bitset iterator
//---------------------------------------------------------------------------

#include "spqopt/base/CColRefSetIter.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/base/CColumnFactory.h"



using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CColRefSetIter::CColRefSetIter
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CColRefSetIter::CColRefSetIter(const CColRefSet &bs) : CBitSetIter(bs)
{
	// get column factory from optimizer context object
	m_pcf = COptCtxt::PoctxtFromTLS()->Pcf();
	SPQOS_ASSERT(NULL != m_pcf);
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSetIter::Pcr
//
//	@doc:
//		Return colref of current position of cursor
//
//---------------------------------------------------------------------------
CColRef *
CColRefSetIter::Pcr() const
{
	ULONG id = CBitSetIter::Bit();

	// resolve id through column factory
	return m_pcf->LookupColRef(id);
}

// EOF
