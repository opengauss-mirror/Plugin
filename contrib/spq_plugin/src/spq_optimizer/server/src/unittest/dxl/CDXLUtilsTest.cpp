//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDXLUtilsTest.cpp
//
//	@doc:
//		Tests DXL utility functions
//---------------------------------------------------------------------------

#include "unittest/dxl/CDXLUtilsTest.h"

#include <xercesc/util/Base64.hpp>

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/CRandom.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "naucrates/base/CQueryToDXLResult.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

XERCES_CPP_NAMESPACE_USE

using namespace spqos;
using namespace spqdxl;

static const char *szQueryFile =
	"../data/dxl/expressiontests/TableScanQuery.xml";
static const char *szPlanFile = "../data/dxl/expressiontests/TableScanPlan.xml";

//---------------------------------------------------------------------------
//	@function:
//		CDXLUtilsTest::EresUnittest
//
//	@doc:
//		Unittest for serializing XML
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDXLUtilsTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CDXLUtilsTest::EresUnittest_SerializeQuery),
		SPQOS_UNITTEST_FUNC(CDXLUtilsTest::EresUnittest_SerializePlan),
		SPQOS_UNITTEST_FUNC(CDXLUtilsTest::EresUnittest_Encoding),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLUtilsTest::EresUnittest_SerializeQuery
//
//	@doc:
//		Testing serialization of queries in DXL
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDXLUtilsTest::EresUnittest_SerializeQuery()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// read DXL file
	CHAR *dxl_string = CDXLUtils::Read(mp, szQueryFile);

	CQueryToDXLResult *presult = CDXLUtils::ParseQueryToQueryDXLTree(
		mp, dxl_string, NULL /*xsd_file_path*/);

	// serialize with document header
	BOOL rgfIndentation[] = {true, false};
	BOOL rgfHeaders[] = {true, false};

	CWStringDynamic str(mp);
	COstreamString oss(&str);

	for (ULONG ulHeaders = 0; ulHeaders < SPQOS_ARRAY_SIZE(rgfHeaders);
		 ulHeaders++)
	{
		for (ULONG ulIndent = 0; ulIndent < SPQOS_ARRAY_SIZE(rgfIndentation);
			 ulIndent++)
		{
			oss << "Headers: " << rgfHeaders[ulHeaders]
				<< ", indentation: " << rgfIndentation[ulIndent] << std::endl;
			CDXLUtils::SerializeQuery(mp, oss, presult->CreateDXLNode(),
									  presult->GetOutputColumnsDXLArray(),
									  presult->GetCTEProducerDXLArray(),
									  rgfHeaders[ulHeaders],
									  rgfIndentation[ulIndent]);
			oss << std::endl;
		}
	}


	SPQOS_TRACE(str.GetBuffer());

	// cleanup
	SPQOS_DELETE(presult);
	SPQOS_DELETE_ARRAY(dxl_string);

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLUtilsTest::EresUnittest_SerializePlan
//
//	@doc:
//		Testing serialization of plans in DXL
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDXLUtilsTest::EresUnittest_SerializePlan()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// read DXL file
	CHAR *dxl_string = CDXLUtils::Read(mp, szPlanFile);

	ULLONG plan_id = spqos::ullong_max;
	ULLONG plan_space_size = spqos::ullong_max;
	CDXLNode *node = CDXLUtils::GetPlanDXLNode(
		mp, dxl_string, NULL /*xsd_file_path*/, &plan_id, &plan_space_size);

	// serialize with document header
	BOOL rgfIndentation[] = {true, false};
	BOOL rgfHeaders[] = {true, false};

	CWStringDynamic str(mp);
	COstreamString oss(&str);

	for (ULONG ulHeaders = 0; ulHeaders < SPQOS_ARRAY_SIZE(rgfHeaders);
		 ulHeaders++)
	{
		for (ULONG ulIndent = 0; ulIndent < SPQOS_ARRAY_SIZE(rgfIndentation);
			 ulIndent++)
		{
			oss << "Headers: " << rgfHeaders[ulHeaders]
				<< ", indentation: " << rgfIndentation[ulIndent] << std::endl;
			CDXLUtils::SerializePlan(mp, oss, node, plan_id, plan_space_size,
									 rgfHeaders[ulHeaders],
									 rgfIndentation[ulIndent]);
			oss << std::endl;
		}
	}


	SPQOS_TRACE(str.GetBuffer());

	// cleanup
	node->Release();
	SPQOS_DELETE_ARRAY(dxl_string);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CDXLUtilsTest::EresUnittest_Encoding
//
//	@doc:
//		Testing base64 encoding
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDXLUtilsTest::EresUnittest_Encoding()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoP<CDXLMemoryManager> a_pmm(SPQOS_NEW(mp) CDXLMemoryManager(mp));

	const CHAR *sz =
		"{\"{FUNCEXPR :funcid 1967 :funcresulttype 1184 :funcretset false :funcformat 1 :args ({FUNCEXPR :funcid 1191 :funcresulttype 1184 :funcretset false :funcformat 2 :args ({CONST :consttype 25 :constlen -1 :constbyval false :constisnull false :constvalue 7 [ 0 0 0 7 110 111 119 ]})} {CONST :consttype 23 :constlen 4 :constbyval true :constisnull false :constvalue 4 [ 2 0 0 0 0 0 0 0 ]})}\"}";
	ULONG len = clib::Strlen(sz);
	const XMLByte *pxmlbyte = (const XMLByte *) sz;

	// encode string in base 64
	XMLSize_t output_length = 0;
	CAutoRg<XMLByte> a_pxmlbyteEncoded;
	a_pxmlbyteEncoded = Base64::encode(pxmlbyte, (XMLSize_t) len,
									   &output_length, a_pmm.Value());
	CHAR *szEncoded = (CHAR *) (a_pxmlbyteEncoded.Rgt());

	// convert encoded string to array of XMLCh
	XMLCh *pxmlch = SPQOS_NEW_ARRAY(mp, XMLCh, output_length + 1);
	for (ULONG ul = 0; ul < output_length; ul++)
	{
		pxmlch[ul] = (XMLCh) a_pxmlbyteEncoded[ul];
	}
	pxmlch[output_length] = 0;

	// decode encoded string
	CAutoRg<XMLByte> a_pxmlbyteDecoded;
	a_pxmlbyteDecoded =
		Base64::decode(a_pxmlbyteEncoded.Rgt(), &output_length, a_pmm.Value());
	CHAR *szDecoded = (CHAR *) (a_pxmlbyteDecoded.Rgt());
	SPQOS_ASSERT(0 == clib::Strcmp(szDecoded, sz));

	// get a byte array from XMLCh representation of encoded string
	ULONG ulOutputLen = 0;
	BYTE *byte = CDXLUtils::CreateStringFrom64XMLStr(a_pmm.Value(), pxmlch,
													 &ulOutputLen);
	CHAR *szPba = (CHAR *) byte;
	SPQOS_ASSERT(0 == clib::Strcmp(szPba, sz));

	{
		CAutoTrace at(mp);
		at.Os() << std::endl << "Input:" << sz << std::endl;
		at.Os() << std::endl << "Encoded:" << szEncoded << std::endl;
		at.Os() << std::endl << "Decoded:" << szDecoded << std::endl;
		at.Os() << std::endl
				<< "Decoded from byte array:" << szPba << std::endl;
	}

	SPQOS_DELETE_ARRAY(byte);
	SPQOS_DELETE_ARRAY(pxmlch);

	return SPQOS_OK;
}

// EOF
