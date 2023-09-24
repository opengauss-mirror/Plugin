//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformFactoryTest.cpp
//
//	@doc:
//		Unittests for management of the global xform set
//---------------------------------------------------------------------------

#include "unittest/spqopt/xforms/CXformFactoryTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/xforms/xforms.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformFactoryTest::EresUnittest
//
//	@doc:
//		Unittest for xform factory
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformFactoryTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CXformFactoryTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CXformFactoryTest::EresUnittest_Basic
//
//	@doc:
//		create factory and instantiate
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformFactoryTest::EresUnittest_Basic()
{
#ifdef SPQOS_DEBUG

	CXform *pxf = CXformFactory::Pxff()->Pxf(CXform::ExfGet2TableScan);
	SPQOS_ASSERT(CXform::ExfGet2TableScan == pxf->Exfid());

	pxf = CXformFactory::Pxff()->Pxf(CXform::ExfInnerJoin2NLJoin);
	SPQOS_ASSERT(CXform::ExfInnerJoin2NLJoin == pxf->Exfid());

	pxf = CXformFactory::Pxff()->Pxf(CXform::ExfGbAgg2HashAgg);
	SPQOS_ASSERT(CXform::ExfGbAgg2HashAgg == pxf->Exfid());

	pxf = CXformFactory::Pxff()->Pxf(CXform::ExfJoinCommutativity);
	SPQOS_ASSERT(CXform::ExfJoinCommutativity == pxf->Exfid());

	pxf = CXformFactory::Pxff()->Pxf(CXform::ExfJoinAssociativity);
	SPQOS_ASSERT(CXform::ExfJoinAssociativity == pxf->Exfid());

#endif	// SPQOS_DEBUG

	return SPQOS_OK;
}



// EOF
