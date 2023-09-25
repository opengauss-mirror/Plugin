//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CListTest.h
//
//	@doc:
//		Tests for CList
//---------------------------------------------------------------------------
#ifndef SPQOS_CListTest_H
#define SPQOS_CListTest_H

#include "spqos/common/CList.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CListTest
//
//	@doc:
//		Wrapper class for CList template to avoid compiler confusion regarding
//		instantiation with sample parameters
//
//---------------------------------------------------------------------------
class CListTest
{
public:
	//---------------------------------------------------------------------------
	//	@class:
	//		SElem
	//
	//	@doc:
	//		Local class for list experiment;
	//
	//---------------------------------------------------------------------------
	struct SElem
	{
	public:
		// generic link for primary list
		SLink m_linkFwd;

		// ..for secondary list
		SLink m_linkBwd;

	};	// struct SElem

	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();
	static SPQOS_RESULT EresUnittest_Navigate();
	static SPQOS_RESULT EresUnittest_Cursor();

};	// class CListTest
}  // namespace spqos


#endif	// !SPQOS_CListTest_H

// EOF
