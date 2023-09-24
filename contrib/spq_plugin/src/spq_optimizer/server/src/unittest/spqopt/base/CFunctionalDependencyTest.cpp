//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CFunctionalDependencyTest.cpp
//
//	@doc:
//		Tests for functional dependencies
//---------------------------------------------------------------------------
#include "unittest/spqopt/base/CFunctionalDependencyTest.h"

#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CFunctionalDependency.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependencyTest::EresUnittest
//
//	@doc:
//		Unittest for functional dependencies
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFunctionalDependencyTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CFunctionalDependencyTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependencyTest::EresUnittest_Basics
//
//	@doc:
//		Basic test for functional dependencies
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFunctionalDependencyTest::EresUnittest_Basics()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CWStringConst strName(SPQOS_WSZ_LIT("Test Column"));
	CName name(&strName);

	const IMDTypeInt4 *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();

	const ULONG num_cols = 3;
	CColRefSet *pcrsLeft = SPQOS_NEW(mp) CColRefSet(mp);
	CColRefSet *pcrsRight = SPQOS_NEW(mp) CColRefSet(mp);
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColRef *colref =
			col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
		pcrsLeft->Include(colref);

		colref =
			col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
		pcrsRight->Include(colref);
	}

	pcrsLeft->AddRef();
	pcrsRight->AddRef();
	CFunctionalDependency *pfdFst =
		SPQOS_NEW(mp) CFunctionalDependency(pcrsLeft, pcrsRight);

	pcrsLeft->AddRef();
	pcrsRight->AddRef();
	CFunctionalDependency *pfdSnd =
		SPQOS_NEW(mp) CFunctionalDependency(pcrsLeft, pcrsRight);

	SPQOS_ASSERT(pfdFst->Equals(pfdSnd));
	SPQOS_ASSERT(pfdFst->HashValue() == pfdSnd->HashValue());

	CFunctionalDependencyArray *pdrspqfd =
		SPQOS_NEW(mp) CFunctionalDependencyArray(mp);
	pfdFst->AddRef();
	pdrspqfd->Append(pfdFst);
	pfdSnd->AddRef();
	pdrspqfd->Append(pfdSnd);
	SPQOS_ASSERT(CFunctionalDependency::Equals(pdrspqfd, pdrspqfd));

	CColRefArray *colref_array =
		CFunctionalDependency::PdrspqcrKeys(mp, pdrspqfd);
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(colref_array);
	CColRefSet *pcrsKeys = CFunctionalDependency::PcrsKeys(mp, pdrspqfd);

	SPQOS_ASSERT(pcrsLeft->Equals(pcrs));
	SPQOS_ASSERT(pcrsKeys->Equals(pcrs));

	CAutoTrace at(mp);
	at.Os() << "FD1:" << *pfdFst << std::endl << "FD2:" << *pfdSnd << std::endl;

	pfdFst->Release();
	pfdSnd->Release();
	pcrsLeft->Release();
	pcrsRight->Release();
	pdrspqfd->Release();
	colref_array->Release();
	pcrs->Release();
	pcrsKeys->Release();

	return SPQOS_OK;
}


// EOF
