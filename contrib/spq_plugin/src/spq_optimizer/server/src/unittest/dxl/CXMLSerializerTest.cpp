//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CXMLSerializerTest.cpp
//
//	@doc:
//		Tests serializing XML.
//---------------------------------------------------------------------------

#include "unittest/dxl/CXMLSerializerTest.h"

#include "spqos/base.h"
#include "spqos/common/CRandom.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CXMLSerializerTest::EresUnittest
//
//	@doc:
//		Unittest for serializing XML
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXMLSerializerTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CXMLSerializerTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CXMLSerializerTest::EresUnittest_Base64)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CXMLSerializerTest::GetMDName
//
//	@doc:
//		Generate an XML string with or without identation using the XML serializer
//
//---------------------------------------------------------------------------
CWStringDynamic *
CXMLSerializerTest::Pstr(CMemoryPool *mp, BOOL indentation)
{
	CWStringDynamic *str = SPQOS_NEW(mp) CWStringDynamic(mp);

	// create a string stream to hold the result of serialization
	COstreamString oss(str);

	CXMLSerializer xml_serializer(mp, oss, indentation);

	xml_serializer.StartDocument();

	xml_serializer.OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenPlan));

	CWStringConst strSubplan(SPQOS_WSZ_LIT("Subplan"));
	xml_serializer.OpenElement(NULL, &strSubplan);
	xml_serializer.CloseElement(NULL, &strSubplan);

	xml_serializer.CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenPlan));

	return str;
}
//---------------------------------------------------------------------------
//	@function:
//		CXMLSerializerTest::EresUnittest_Basic
//
//	@doc:
//		Testing XML serialization
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXMLSerializerTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// test XML serializer with indentation
	CWStringDynamic *pstrIndented = Pstr(mp, true /* indentation */);

	CWStringConst strExpectedIndented(SPQOS_WSZ_LIT(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<dxl:Plan>\n  <Subplan/>\n</dxl:Plan>\n"));

	SPQOS_RESULT eresIndented = SPQOS_FAILED;

	if (pstrIndented->Equals(&strExpectedIndented))
	{
		eresIndented = SPQOS_OK;
	}

	// test XML serializer without indentation
	CWStringDynamic *pstrNotIndented = Pstr(mp, false /* indentation */);

	CWStringConst strExpectedNotIndented(SPQOS_WSZ_LIT(
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?><dxl:Plan><Subplan/></dxl:Plan>"));

	SPQOS_RESULT eresNotIndented = SPQOS_FAILED;

	if (pstrNotIndented->Equals(&strExpectedNotIndented))
	{
		eresNotIndented = SPQOS_OK;
	}

	SPQOS_DELETE(pstrIndented);
	SPQOS_DELETE(pstrNotIndented);

	if (SPQOS_FAILED == eresIndented || SPQOS_FAILED == eresNotIndented)
	{
		return SPQOS_FAILED;
	}
	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CXMLSerializerTest::EresUnittest_Base64
//
//	@doc:
//		Testing base64 encoding and decoding
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXMLSerializerTest::EresUnittest_Base64()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	const ULONG ulraSize = 5;
	ULONG rgulRandArr[ulraSize];

	CRandom cr;
	for (ULONG i = 0; i < ulraSize; i++)
	{
		rgulRandArr[i] = cr.Next();
	}

	CWStringDynamic *str = CDXLUtils::EncodeByteArrayToString(
		mp, (BYTE *) rgulRandArr, sizeof(rgulRandArr));

	ULONG len;

	ULONG *pulRandArrCopy =
		(ULONG *) CDXLUtils::DecodeByteArrayFromString(mp, str, &len);

	SPQOS_ASSERT(len == sizeof(rgulRandArr));

	for (ULONG i = 0; i < ulraSize; i++)
	{
		if (rgulRandArr[i] != pulRandArrCopy[i])
		{
			return SPQOS_FAILED;
		}
	}

	SPQOS_DELETE(str);
	SPQOS_DELETE_ARRAY(pulRandArrCopy);

	INT i = 1000;
	str = CDXLUtils::EncodeByteArrayToString(mp, (BYTE *) &i, sizeof(i));

	spqos::oswcout << "Base64 encoding of " << i << " is " << str->GetBuffer()
				  << std::endl;

	SPQOS_DELETE(str);

	return SPQOS_OK;
}

// EOF
