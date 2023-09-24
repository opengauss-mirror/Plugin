//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerTest.h
//
//	@doc:
//		Tests parsing of DXL into DXL trees
//---------------------------------------------------------------------------


#ifndef SPQOPT_CParseHandlerTest_H
#define SPQOPT_CParseHandlerTest_H

#include "spqos/base.h"


namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CParseHandlerTest
{
private:
	// metadata request file
	static const CHAR *m_szMDRequestFile;

	// files for testing parsing of different DXL nodes
	static const CHAR *m_rgszPlanDXLFileNames[];

	// files for tests involving dxl representation of queries
	static const CHAR *m_rgszQueryDXLFileNames[];

	// files for the statistics tests
	static const CHAR *m_rgszStatsDXLFileNames[];

	// files for the metadata tests
	static const CHAR *m_rgszMetadataDXLFileNames[];

	// input files for scalar expression tests
	static const CHAR *m_rgszScalarExprDXLFileNames[];

	// files for testing Xerces
	static const CHAR *m_rgszXerceTestFileNames[];

	// files for testing exception handling in DXL parsing
	static const CHAR *m_rgszNegativeTestsFileNames[];

	// test parsing and serializing of plans
	static SPQOS_RESULT EresParseAndSerializePlan(CMemoryPool *, const CHAR *,
												 BOOL fValidate);

	// test parsing and serializing of queries
	static SPQOS_RESULT EresParseAndSerializeQuery(CMemoryPool *, const CHAR *,
												  BOOL fValidate);

	// test parsing and serializing of metadata
	static SPQOS_RESULT EresParseAndSerializeMetadata(CMemoryPool *,
													 const CHAR *,
													 BOOL fValidate);

	// test parsing and serializing of metadata requests
	static SPQOS_RESULT EresParseAndSerializeMDRequest(CMemoryPool *,
													  const CHAR *,
													  BOOL fValidate);

	// test parsing and serializing of stats
	static SPQOS_RESULT EresParseAndSerializeStatistics(CMemoryPool *,
													   const CHAR *,
													   BOOL fValidate);

	// test parsing and serializing of scalar expressions
	static SPQOS_RESULT EresParseAndSerializeScalarExpr(CMemoryPool *,
													   const CHAR *,
													   BOOL fValidate);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	// run all plan tests
	static SPQOS_RESULT EresUnittest_RunPlanTests();

	// run all positive tests
	static SPQOS_RESULT EresUnittest_RunAllPositiveTests(
		const CHAR *rgszFileNames[], ULONG ulFiles,
		SPQOS_RESULT (*testFunc)(CMemoryPool *, const CHAR *, BOOL),
		BOOL fValidate);

	// run medata tests
	static SPQOS_RESULT EresUnittest_Metadata();

	// run MD request test
	static SPQOS_RESULT EresUnittest_MDRequest();

	// run stats test
	static SPQOS_RESULT EresUnittest_Statistics();

	// run scalar expression tests
	static SPQOS_RESULT EresUnittest_ScalarExpr();

	// tests checking handling exception cases (e.g. due to invalid DXL
	// or unsupported operators)
	static SPQOS_RESULT EresUnittest_ErrSAXParseException();

	// tests checking the parsing DXL representing queries
	static SPQOS_RESULT EresUnittest_RunQueryTests();

	// tests checking the "failing" tests
	static SPQOS_RESULT EresUnittest_RunAllNegativeTests();

};	// class CParseHandlerTest
}  // namespace spqdxl

#endif	// !SPQOPT_CParseHandlerTest_H

// EOF
