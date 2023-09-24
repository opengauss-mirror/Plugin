//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CXMLSerializerTest.h
//
//	@doc:
//		Tests the XML serializer
//---------------------------------------------------------------------------


#ifndef SPQOPT_CXMLSerializerTest_H
#define SPQOPT_CXMLSerializerTest_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"

// fwd decl
namespace spqos
{
class CWStringDynamic;
}

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXMLSerializerTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CXMLSerializerTest
{
private:
	// helper function for using the XML serializer to generate a document
	// with or without indentation
	static CWStringDynamic *Pstr(CMemoryPool *mp, BOOL indentation);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_NoIndent();
	static SPQOS_RESULT EresUnittest_Base64();

};	// class CXMLSerializerTest
}  // namespace spqdxl

#endif	// !SPQOPT_CXMLSerializerTest_H

// EOF
