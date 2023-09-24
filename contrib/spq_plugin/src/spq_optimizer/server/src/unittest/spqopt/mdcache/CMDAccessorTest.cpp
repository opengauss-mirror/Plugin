//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMDAccessorTest.cpp
//
//	@doc:
//		Tests accessing objects from the metadata cache.
//---------------------------------------------------------------------------

#include "unittest/spqopt/mdcache/CMDAccessorTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CCacheFactory.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/task/CAutoTaskProxy.h"

#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/base/IDatumBool.h"
#include "naucrates/base/IDatumInt4.h"
#include "naucrates/base/IDatumOid.h"
#include "naucrates/exception.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDAggregate.h"
#include "naucrates/md/IMDCast.h"
#include "naucrates/md/IMDCheckConstraint.h"
#include "naucrates/md/IMDColumn.h"
#include "naucrates/md/IMDFunction.h"
#include "naucrates/md/IMDIndex.h"
#include "naucrates/md/IMDPartConstraint.h"
#include "naucrates/md/IMDRelation.h"
#include "naucrates/md/IMDScCmp.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDTypeBool.h"
#include "naucrates/md/IMDTypeGeneric.h"
#include "naucrates/md/IMDTypeInt4.h"
#include "naucrates/md/IMDTypeOid.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/mdcache/CMDProviderTest.h"


#define SPQOPT_MDCACHE_LOOKUP_THREADS 8
#define SPQOPT_MDCACHE_MDAS 4

#define SPQDB_AGG_AVG OID(2101)
#define SPQDB_OP_INT4_LT OID(97)
#define SPQDB_FUNC_TIMEOFDAY OID(274)

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest
//
//	@doc:
//
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_Datum),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC_ASSERT(CMDAccessorTest::EresUnittest_DatumGeneric),
#endif
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_Navigate),
		SPQOS_UNITTEST_FUNC_THROW(CMDAccessorTest::EresUnittest_Negative,
								 spqdxl::ExmaMD,
								 spqdxl::ExmiMDCacheEntryNotFound),
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_Indexes),
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_CheckConstraint),
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_IndexPartConstraint),
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_Cast),
		SPQOS_UNITTEST_FUNC(CMDAccessorTest::EresUnittest_ScCmp)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_Basic
//
//	@doc:
//		Test fetching metadata objects from a metadata cache
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// lookup different objects
	CMDIdSPQDB *pmdidObject1 = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID /* OID */,
				  1 /* major version */, 1 /* minor version */);
	CMDIdSPQDB *pmdidObject2 = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID /* OID */,
				  12 /* version */, 1 /* minor version */);

#ifdef SPQOS_DEBUG
	const IMDRelation *pimdrel1 =
#endif
		mda.RetrieveRel(pmdidObject1);

#ifdef SPQOS_DEBUG
	const IMDRelation *pimdrel2 =
#endif
		mda.RetrieveRel(pmdidObject2);

	SPQOS_ASSERT(pimdrel1->MDId()->Equals(pmdidObject1) &&
				pimdrel2->MDId()->Equals(pmdidObject2));

	// access an object again
#ifdef SPQOS_DEBUG
	const IMDRelation *pimdrel3 =
#endif
		mda.RetrieveRel(pmdidObject1);

	SPQOS_ASSERT(pimdrel1 == pimdrel3);

	// access SPQDB types, operators and aggregates
	CMDIdSPQDB *mdid_type =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4, 1, 0);
	CMDIdSPQDB *mdid_op =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_OP_INT4_LT, 1, 0);
	CMDIdSPQDB *agg_mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_AGG_AVG, 1, 0);

#ifdef SPQOS_DEBUG
	const IMDType *pimdtype =
#endif
		mda.RetrieveType(mdid_type);

#ifdef SPQOS_DEBUG
	const IMDScalarOp *md_scalar_op =
#endif
		mda.RetrieveScOp(mdid_op);

	SPQOS_ASSERT(IMDType::EcmptL == md_scalar_op->ParseCmpType());

#ifdef SPQOS_DEBUG
	const IMDAggregate *pmdagg =
#endif
		mda.RetrieveAgg(agg_mdid);


	// access types by type info
#ifdef SPQOS_DEBUG
	const IMDTypeInt4 *pmdtypeint4 =
#endif
		mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);

#ifdef SPQOS_DEBUG
	const IMDTypeBool *pmdtypebool =
#endif
		mda.PtMDType<IMDTypeBool>(CTestUtils::m_sysidDefault);


#ifdef SPQOS_DEBUG
	// for debug traces
	CAutoTrace at(mp);
	IOstream &os(at.Os());

	// print objects
	os << std::endl;
	pimdrel1->DebugPrint(os);
	os << std::endl;

	pimdrel2->DebugPrint(os);
	os << std::endl;

	pimdtype->DebugPrint(os);
	os << std::endl;

	md_scalar_op->DebugPrint(os);
	os << std::endl;

	pmdagg->DebugPrint(os);
	os << std::endl;

	pmdtypeint4->DebugPrint(os);
	os << std::endl;

	pmdtypebool->DebugPrint(os);
	os << std::endl;

#endif

	pmdidObject1->Release();
	pmdidObject2->Release();
	agg_mdid->Release();
	mdid_op->Release();
	mdid_type->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_Datum
//
//	@doc:
//		Test type factory method for creating base type datums
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_Datum()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// create an INT4 datum
	const IMDTypeInt4 *pmdtypeint4 =
		mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);
	IDatumInt4 *pdatumInt4 =
		pmdtypeint4->CreateInt4Datum(mp, 5, false /* is_null */);
	SPQOS_ASSERT(5 == pdatumInt4->Value());

	// create a BOOL datum
	const IMDTypeBool *pmdtypebool =
		mda.PtMDType<IMDTypeBool>(CTestUtils::m_sysidDefault);
	IDatumBool *pdatumBool =
		pmdtypebool->CreateBoolDatum(mp, false, false /* is_null */);
	SPQOS_ASSERT(false == pdatumBool->GetValue());

	// create an OID datum
	const IMDTypeOid *pmdtypeoid =
		mda.PtMDType<IMDTypeOid>(CTestUtils::m_sysidDefault);
	IDatumOid *pdatumOid =
		pmdtypeoid->CreateOidDatum(mp, 20, false /* is_null */);
	SPQOS_ASSERT(20 == pdatumOid->OidValue());


#ifdef SPQOS_DEBUG
	// for debug traces
	CWStringDynamic str(mp);
	COstreamString oss(&str);

	// print objects
	oss << std::endl;
	oss << "Int4 datum" << std::endl;
	pdatumInt4->OsPrint(oss);

	oss << std::endl;
	oss << "Bool datum" << std::endl;
	pdatumBool->OsPrint(oss);

	oss << std::endl;
	oss << "Oid datum" << std::endl;
	pdatumOid->OsPrint(oss);

	SPQOS_TRACE(str.GetBuffer());
#endif

	// cleanup
	pdatumInt4->Release();
	pdatumBool->Release();
	pdatumOid->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_DatumGeneric
//
//	@doc:
//		Test asserting during an attempt to obtain a generic type from the cache
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_DatumGeneric()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// attempt to obtain a generic type from the cache should assert
	(void) mda.PtMDType<IMDTypeGeneric>();

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_Navigate
//
//	@doc:
//		Test fetching a MD object from the cache and navigating its dependent
//		objects
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_Navigate()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();

	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// lookup a function in the MD cache
	CMDIdSPQDB *mdid_func = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_FUNC_TIMEOFDAY /* OID */,
				  1 /* major version */, 0 /* minor version */);

	const IMDFunction *pmdfunc = mda.RetrieveFunc(mdid_func);

	// lookup function return type
	IMDId *pmdidFuncReturnType = pmdfunc->GetResultTypeMdid();
	const IMDType *pimdtype = mda.RetrieveType(pmdidFuncReturnType);

	// lookup equality operator for function return type
	IMDId *pmdidEqOp = pimdtype->GetMdidForCmpType(IMDType::EcmptEq);

#ifdef SPQOS_DEBUG
	const IMDScalarOp *md_scalar_op =
#endif
		mda.RetrieveScOp(pmdidEqOp);

#ifdef SPQOS_DEBUG
	// print objects
	CWStringDynamic str(mp);
	COstreamString oss(&str);

	oss << std::endl;
	oss << std::endl;

	pmdfunc->DebugPrint(oss);
	oss << std::endl;

	pimdtype->DebugPrint(oss);
	oss << std::endl;

	md_scalar_op->DebugPrint(oss);
	oss << std::endl;

	SPQOS_TRACE(str.GetBuffer());

#endif

	mdid_func->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_Indexes
//
//	@doc:
//		Test fetching indexes from the cache
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_Indexes()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTrace at(mp);

#ifdef SPQOS_DEBUG
	IOstream &os(at.Os());
#endif	// SPQOS_DEBUG

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// lookup a relation in the MD cache
	CMDIdSPQDB *rel_mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID,
							   1 /* major */, 1 /* minor version */);

	const IMDRelation *pmdrel = mda.RetrieveRel(rel_mdid);

	SPQOS_ASSERT(0 < pmdrel->IndexCount());

	IMDId *pmdidIndex = pmdrel->IndexMDidAt(0);
	const IMDIndex *pmdindex = mda.RetrieveIndex(pmdidIndex);

	ULONG ulKeys = pmdindex->Keys();

#ifdef SPQOS_DEBUG
	// print index
	pmdindex->DebugPrint(os);

	os << std::endl;
	os << "Index columns: " << std::endl;

#endif	// SPQOS_DEBUG

	for (ULONG ul = 0; ul < ulKeys; ul++)
	{
		ULONG ulKeyColumn = pmdindex->KeyAt(ul);

#ifdef SPQOS_DEBUG
		const IMDColumn *pmdcol =
#endif	// SPQOS_DEBUG
			pmdrel->GetMdCol(ulKeyColumn);

#ifdef SPQOS_DEBUG
		pmdcol->DebugPrint(os);
#endif	// SPQOS_DEBUG
	}

	rel_mdid->Release();
	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_CheckConstraint
//
//	@doc:
//		Test fetching check constraint from the cache and translating the
//		check constraint scalar expression
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_CheckConstraint()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTrace at(mp);

#ifdef SPQOS_DEBUG
	IOstream &os(at.Os());
#endif	// SPQOS_DEBUG

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	// lookup a relation in the MD cache
	CMDIdSPQDB *rel_mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID21,
							   1 /* major */, 1 /* minor version */);

	const IMDRelation *pmdrel = mda.RetrieveRel(rel_mdid);
	SPQOS_ASSERT(0 < pmdrel->CheckConstraintCount());

	// create the array of column reference for the table columns
	// for the DXL to Expr translation
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	const ULONG num_cols = pmdrel->ColumnCount() - pmdrel->SystemColumnsCount();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		const IMDColumn *pmdcol = pmdrel->GetMdCol(ul);
		const IMDType *pmdtype = mda.RetrieveType(pmdcol->MdidType());
		CColRef *colref =
			col_factory->PcrCreate(pmdtype, pmdcol->TypeModifier());
		colref_array->Append(colref);
	}

	// get one of its check constraint
	IMDId *pmdidCheckConstraint = pmdrel->CheckConstraintMDidAt(0);
	const IMDCheckConstraint *pmdCheckConstraint =
		mda.RetrieveCheckConstraints(pmdidCheckConstraint);

#ifdef SPQOS_DEBUG
	os << std::endl;
	// print check constraint
	pmdCheckConstraint->DebugPrint(os);
	os << std::endl;
#endif	// SPQOS_DEBUG

	// extract and then print the check constraint expression
	CExpression *pexpr =
		pmdCheckConstraint->GetCheckConstraintExpr(mp, &mda, colref_array);

#ifdef SPQOS_DEBUG
	pexpr->DbgPrint();
	os << std::endl;
#endif	// SPQOS_DEBUG

	// clean up
	pexpr->Release();
	colref_array->Release();
	rel_mdid->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_IndexPartConstraint
//
//	@doc:
//		Test fetching part constraints for indexes on partitioned tables
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_IndexPartConstraint()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTrace at(mp);

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	// lookup a relation in the MD cache
	CMDIdSPQDB *rel_mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID22);

	const IMDRelation *pmdrel = mda.RetrieveRel(rel_mdid);
	SPQOS_ASSERT(0 < pmdrel->IndexCount());

	// create the array of column reference for the table columns
	// for the DXL to Expr translation
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	const ULONG num_cols = pmdrel->ColumnCount() - pmdrel->SystemColumnsCount();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		const IMDColumn *pmdcol = pmdrel->GetMdCol(ul);
		const IMDType *pmdtype = mda.RetrieveType(pmdcol->MdidType());
		CColRef *colref =
			col_factory->PcrCreate(pmdtype, pmdcol->TypeModifier());
		colref_array->Append(colref);
	}

	// get one of its indexes
	SPQOS_ASSERT(0 < pmdrel->IndexCount());
	IMDId *pmdidIndex = pmdrel->IndexMDidAt(0);
	const IMDIndex *pmdindex = mda.RetrieveIndex(pmdidIndex);

	// extract and then print the part constraint expression
	IMDPartConstraint *mdpart_constraint = pmdindex->MDPartConstraint();
	SPQOS_ASSERT(NULL != mdpart_constraint);

	CExpression *pexpr =
		mdpart_constraint->GetPartConstraintExpr(mp, &mda, colref_array);

#ifdef SPQOS_DEBUG
	IOstream &os(at.Os());

	pexpr->DbgPrint();
	os << std::endl;
#endif	// SPQOS_DEBUG

	// clean up
	pexpr->Release();
	colref_array->Release();
	rel_mdid->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_Cast
//
//	@doc:
//		Test fetching cast objects from the MD cache
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_Cast()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTrace at(mp);

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	const IMDType *pmdtypeInt =
		mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);
	const IMDType *pmdtypeOid =
		mda.PtMDType<IMDTypeOid>(CTestUtils::m_sysidDefault);
	const IMDType *pmdtypeBigInt =
		mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);

#ifdef SPQOS_DEBUG
	const IMDCast *pmdcastInt2BigInt =
#endif	// SPQOS_DEBUG
		mda.Pmdcast(pmdtypeInt->MDId(), pmdtypeBigInt->MDId());

	SPQOS_ASSERT(!pmdcastInt2BigInt->IsBinaryCoercible());
	SPQOS_ASSERT(pmdcastInt2BigInt->GetCastFuncMdId()->IsValid());
	SPQOS_ASSERT(pmdcastInt2BigInt->MdidSrc()->Equals(pmdtypeInt->MDId()));
	SPQOS_ASSERT(pmdcastInt2BigInt->MdidDest()->Equals(pmdtypeBigInt->MDId()));

#ifdef SPQOS_DEBUG
	const IMDCast *pmdcastInt2Oid =
#endif	// SPQOS_DEBUG
		mda.Pmdcast(pmdtypeInt->MDId(), pmdtypeOid->MDId());

	SPQOS_ASSERT(pmdcastInt2Oid->IsBinaryCoercible());
	SPQOS_ASSERT(!pmdcastInt2Oid->GetCastFuncMdId()->IsValid());

#ifdef SPQOS_DEBUG
	const IMDCast *pmdcastOid2Int =
#endif	// SPQOS_DEBUG
		mda.Pmdcast(pmdtypeOid->MDId(), pmdtypeInt->MDId());

	SPQOS_ASSERT(pmdcastOid2Int->IsBinaryCoercible());
	SPQOS_ASSERT(!pmdcastOid2Int->GetCastFuncMdId()->IsValid());

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_ScCmp
//
//	@doc:
//		Test fetching scalar comparison objects from the MD cache
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_ScCmp()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTrace at(mp);

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	const IMDType *pmdtypeInt =
		mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);
	const IMDType *pmdtypeBigInt =
		mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);

#ifdef SPQOS_DEBUG
	const IMDScCmp *pmdScEqIntBigInt =
#endif	// SPQOS_DEBUG
		mda.Pmdsccmp(pmdtypeInt->MDId(), pmdtypeBigInt->MDId(),
					 IMDType::EcmptEq);

	SPQOS_ASSERT(IMDType::EcmptEq == pmdScEqIntBigInt->ParseCmpType());
	SPQOS_ASSERT(pmdScEqIntBigInt->GetLeftMdid()->Equals(pmdtypeInt->MDId()));
	SPQOS_ASSERT(
		pmdScEqIntBigInt->GetRightMdid()->Equals(pmdtypeBigInt->MDId()));

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::EresUnittest_Negative
//
//	@doc:
//		Test fetching non-existing metadata objects from the MD cache
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMDAccessorTest::EresUnittest_Negative()
{
	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
	CMemoryPool *mp = amp.Pmp();

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// lookup a non-existing objects
	CMDIdSPQDB *pmdidNonExistingObject = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID /* OID */,
				  15 /* version */, 1 /* minor version */);

	// call should result in an exception
	(void) mda.RetrieveRel(pmdidNonExistingObject);

	pmdidNonExistingObject->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::PvLookupSingleObj
//
//	@doc:
//		A task that looks up a single object from the MD cache
//
//---------------------------------------------------------------------------
void *
CMDAccessorTest::PvLookupSingleObj(void *pv)
{
	SPQOS_CHECK_ABORT;

	SPQOS_ASSERT(NULL != pv);

	SMDCacheTaskParams *pmdtaskparams = (SMDCacheTaskParams *) pv;

	CMDAccessor *md_accessor = pmdtaskparams->m_pmda;

	CMemoryPool *mp = pmdtaskparams->m_mp;

	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != md_accessor);

	// lookup a cache object
	CMDIdSPQDB *mdid = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID /* OID */,
				  1 /* major version */, 1 /* minor version */);

	// lookup object
	(void) md_accessor->RetrieveRel(mdid);
	mdid->Release();

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::PvLookupMultipleObj
//
//	@doc:
//		A task that looks up multiple objects from the MD cache
//
//---------------------------------------------------------------------------
void *
CMDAccessorTest::PvLookupMultipleObj(void *pv)
{
	SPQOS_CHECK_ABORT;

	SPQOS_ASSERT(NULL != pv);

	SMDCacheTaskParams *pmdtaskparams = (SMDCacheTaskParams *) pv;

	CMDAccessor *md_accessor = pmdtaskparams->m_pmda;

	SPQOS_ASSERT(NULL != md_accessor);

	// lookup cache objects
	const ULONG ulNumberOfObjects = 10;

	for (ULONG ul = 0; ul < ulNumberOfObjects; ul++)
	{
		SPQOS_CHECK_ABORT;

		// lookup relation
		CMDIdSPQDB *mdid = SPQOS_NEW(pmdtaskparams->m_mp)
			CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID /*OID*/,
					  1 /*major*/, ul + 1 /*minor*/);
		(void) md_accessor->RetrieveRel(mdid);
		mdid->Release();
	}

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDAccessorTest::PvInitMDAAndLookup
//
//	@doc:
//		Task which initializes a MD accessor and looks up multiple objects through
//		that accessor
//
//---------------------------------------------------------------------------
void *
CMDAccessorTest::PvInitMDAAndLookup(void *pv)
{
	SPQOS_ASSERT(NULL != pv);

	CMDAccessor::MDCache *pcache = (CMDAccessor::MDCache *) pv;

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CWorkerPoolManager *pwpm = CWorkerPoolManager::WorkerPoolManager();

	// task memory pool
	CAutoMemoryPool ampTask;
	CMemoryPool *pmpTask = ampTask.Pmp();

	// scope for MD accessor
	{
		// Setup an MD cache with a file-based provider
		CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
		pmdp->AddRef();
		CMDAccessor mda(mp, pcache, CTestUtils::m_sysidDefault, pmdp);

		// task parameters
		SMDCacheTaskParams mdtaskparams(pmpTask, &mda);

		// scope for ATP
		{
			CAutoTaskProxy atp(mp, pwpm);

			CTask *rgPtsk[SPQOPT_MDCACHE_LOOKUP_THREADS];

			TaskFuncPtr rgPfuncTask[] = {CMDAccessorTest::PvLookupSingleObj,
										 CMDAccessorTest::PvLookupMultipleObj};

			const ULONG ulNumberOfTaskTypes = SPQOS_ARRAY_SIZE(rgPfuncTask);

			// create tasks
			for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rgPtsk); i++)
			{
				rgPtsk[i] = atp.Create(rgPfuncTask[i % ulNumberOfTaskTypes],
									   &mdtaskparams);

				SPQOS_CHECK_ABORT;
			}

			for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rgPtsk); i++)
			{
				atp.Schedule(rgPtsk[i]);
			}

			SPQOS_CHECK_ABORT;

			// wait for completion
			for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rgPtsk); i++)
			{
				atp.Execute(rgPtsk[i]);
				SPQOS_CHECK_ABORT;
			}
		}
	}

	return NULL;
}

// EOF
