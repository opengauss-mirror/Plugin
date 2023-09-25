//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.



#include "unittest/dxl/CParseHandlerCostModelTest.h"

#include <memory>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XercesDefs.hpp>

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/common/CAutoRg.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "spqdbcost/CCostModelSPQDB.h"
#include "spqdbcost/CCostModelParamsSPQDB.h"
#include "naucrates/dxl/CCostModelConfigSerializer.h"
#include "naucrates/dxl/parser/CParseHandlerCostModel.h"
#include "naucrates/dxl/parser/CParseHandlerFactory.h"
#include "naucrates/dxl/parser/CParseHandlerManager.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using spqdbcost::CCostModelSPQDB;
using spqdbcost::CCostModelParamsSPQDB;

XERCES_CPP_NAMESPACE_USE


namespace
{
class Fixture
{
private:
	CAutoMemoryPool m_amp;
	spqos::CAutoP<CDXLMemoryManager> m_apmm;
	std::auto_ptr<SAX2XMLReader> m_apxmlreader;
	spqos::CAutoP<CParseHandlerManager> m_apphm;
	spqos::CAutoP<CParseHandlerCostModel> m_apphCostModel;

public:
	Fixture()
		: m_apmm(SPQOS_NEW(Pmp()) CDXLMemoryManager(Pmp())),
		  m_apxmlreader(
			  XMLReaderFactory::createXMLReader(GetDXLMemoryManager())),
		  m_apphm(SPQOS_NEW(Pmp()) CParseHandlerManager(GetDXLMemoryManager(),
													   Pxmlreader())),
		  m_apphCostModel(SPQOS_NEW(Pmp())
							  CParseHandlerCostModel(Pmp(), Pphm(), NULL))
	{
		m_apphm->ActivateParseHandler(PphCostModel());
	}

	CMemoryPool *
	Pmp() const
	{
		return m_amp.Pmp();
	}

	CDXLMemoryManager *
	GetDXLMemoryManager()
	{
		return m_apmm.Value();
	}

	SAX2XMLReader *
	Pxmlreader()
	{
		return m_apxmlreader.get();
	}

	CParseHandlerManager *
	Pphm()
	{
		return m_apphm.Value();
	}

	CParseHandlerCostModel *
	PphCostModel()
	{
		return m_apphCostModel.Value();
	}

	void
	Parse(const XMLByte dxl_string[], size_t size)
	{
		MemBufInputSource mbis(dxl_string, size, "dxl test", false,
							   GetDXLMemoryManager());
		Pxmlreader()->parse(mbis);
	}
};
}  // namespace

static spqos::SPQOS_RESULT
Eres_ParseCalibratedCostModel()
{
	const CHAR dxl_filename[] =
		"../data/dxl/parse_tests/CostModelConfigCalibrated.xml";
	Fixture fixture;

	CMemoryPool *mp = fixture.Pmp();

	spqos::CAutoRg<CHAR> a_szDXL(CDXLUtils::Read(mp, dxl_filename));

	CParseHandlerCostModel *pphcm = fixture.PphCostModel();

	fixture.Parse((const XMLByte *) a_szDXL.Rgt(), strlen(a_szDXL.Rgt()));

	ICostModel *pcm = pphcm->GetCostModel();

	SPQOS_RTL_ASSERT(ICostModel::EcmtSPQDBCalibrated == pcm->Ecmt());
	SPQOS_RTL_ASSERT(3 == pcm->UlHosts());

	CAutoRef<CCostModelParamsSPQDB> pcpExpected(SPQOS_NEW(mp)
												   CCostModelParamsSPQDB(mp));
	pcpExpected->SetParam(CCostModelParamsSPQDB::EcpNLJFactor, 1024.0, 1023.0,
						  1025.0);
	SPQOS_RTL_ASSERT(pcpExpected->Equals(pcm->GetCostModelParams()));


	return spqos::SPQOS_OK;
}

static spqos::SPQOS_RESULT
Eres_SerializeCalibratedCostModel()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	const WCHAR *const wszExpectedString =
		L"<dxl:CostModelConfig CostModelType=\"1\" SegmentsForCosting=\"3\">"
		"<dxl:CostParams>"
		"<dxl:CostParam Name=\"NLJFactor\" Value=\"1024.000000\" LowerBound=\"1023.000000\" UpperBound=\"1025.000000\"/>"
		"</dxl:CostParams>"
		"</dxl:CostModelConfig>";
	spqos::CAutoP<CWStringDynamic> apwsExpected(
		SPQOS_NEW(mp) CWStringDynamic(mp, wszExpectedString));

	const ULONG ulSegments = 3;
	CCostModelParamsSPQDB *pcp = SPQOS_NEW(mp) CCostModelParamsSPQDB(mp);
	pcp->SetParam(CCostModelParamsSPQDB::EcpNLJFactor, 1024.0, 1023.0, 1025.0);
	spqos::CAutoRef<CCostModelSPQDB> apcm(
		SPQOS_NEW(mp) CCostModelSPQDB(mp, ulSegments, pcp));

	CWStringDynamic wsActual(mp);
	COstreamString os(&wsActual);
	CXMLSerializer xml_serializer(mp, os, false);
	CCostModelConfigSerializer cmcSerializer(apcm.Value());
	cmcSerializer.Serialize(xml_serializer);

	SPQOS_RTL_ASSERT(apwsExpected->Equals(&wsActual));

	return spqos::SPQOS_OK;
}

spqos::SPQOS_RESULT
CParseHandlerCostModelTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(Eres_ParseCalibratedCostModel),
						SPQOS_UNITTEST_FUNC(Eres_SerializeCalibratedCostModel)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}
