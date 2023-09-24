#include "unittest/spqopt/operators/CScalarIsDistinctFromTest.h"

#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/string/CWStringConst.h"

#include "spqopt/operators/CScalarIsDistinctFrom.h"

#include "unittest/spqopt/CTestUtils.h"

namespace spqopt
{
using namespace spqos;

class SEberFixture
{
private:
	const CAutoMemoryPool m_amp;
	CMDAccessor m_mda;
	const CAutoOptCtxt m_aoc;
	CScalarIsDistinctFrom *const m_pScalarIDF;

	static IMDProvider *
	Pmdp()
	{
		CTestUtils::m_pmdpf->AddRef();
		return CTestUtils::m_pmdpf;
	}

public:
	SEberFixture()
		: m_amp(),
		  m_mda(m_amp.Pmp(), CMDCache::Pcache(), CTestUtils::m_sysidDefault,
				Pmdp()),
		  m_aoc(m_amp.Pmp(), &m_mda, NULL /* pceeval */,
				CTestUtils::GetCostModel(m_amp.Pmp())),
		  m_pScalarIDF(SPQOS_NEW(m_amp.Pmp()) CScalarIsDistinctFrom(
			  Pmp(),
			  SPQOS_NEW(m_amp.Pmp())
				  CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_EQ_OP),
			  SPQOS_NEW(m_amp.Pmp()) CWStringConst(SPQOS_WSZ_LIT("="))))
	{
	}

	~SEberFixture()
	{
		m_pScalarIDF->Release();
	}

	CMemoryPool *
	Pmp() const
	{
		return m_amp.Pmp();
	}

	CScalarIsDistinctFrom *
	PScalarIDF() const
	{
		return m_pScalarIDF;
	}
};

static SPQOS_RESULT
EresUnittest_Eber_WhenBothInputsAreNull()
{
	SEberFixture fixture;
	CMemoryPool *mp = fixture.Pmp();

	ULongPtrArray *pdrspqulChildren = SPQOS_NEW(mp) ULongPtrArray(mp);
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberNull));
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberNull));

	CScalarIsDistinctFrom *pScalarIDF = fixture.PScalarIDF();

	CScalar::EBoolEvalResult eberResult = pScalarIDF->Eber(pdrspqulChildren);
	SPQOS_RTL_ASSERT(eberResult == CScalar::EberFalse);

	pdrspqulChildren->Release();

	return SPQOS_OK;
}

static SPQOS_RESULT
EresUnittest_Eber_WhenFirstInputIsUnknown()
{
	SEberFixture fixture;
	CMemoryPool *mp = fixture.Pmp();

	ULongPtrArray *pdrspqulChildren = SPQOS_NEW(mp) ULongPtrArray(mp);
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberAny));
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberNull));

	CScalarIsDistinctFrom *pScalarIDF = fixture.PScalarIDF();

	CScalar::EBoolEvalResult eberResult = pScalarIDF->Eber(pdrspqulChildren);
	SPQOS_RTL_ASSERT(eberResult == CScalar::EberAny);

	pdrspqulChildren->Release();

	return SPQOS_OK;
}

static SPQOS_RESULT
EresUnittest_Eber_WhenSecondInputIsUnknown()
{
	SEberFixture fixture;
	CMemoryPool *mp = fixture.Pmp();

	ULongPtrArray *pdrspqulChildren = SPQOS_NEW(mp) ULongPtrArray(mp);
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberNull));
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberAny));

	CScalarIsDistinctFrom *pScalarIDF = fixture.PScalarIDF();

	CScalar::EBoolEvalResult eberResult = pScalarIDF->Eber(pdrspqulChildren);
	SPQOS_RTL_ASSERT(eberResult == CScalar::EberAny);

	pdrspqulChildren->Release();

	return SPQOS_OK;
}

static SPQOS_RESULT
EresUnittest_Eber_WhenFirstInputDiffersFromSecondInput()
{
	SEberFixture fixture;
	CMemoryPool *mp = fixture.Pmp();

	ULongPtrArray *pdrspqulChildren = SPQOS_NEW(mp) ULongPtrArray(mp);
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberNull));
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberTrue));

	CScalarIsDistinctFrom *pScalarIDF = fixture.PScalarIDF();

	CScalar::EBoolEvalResult eberResult = pScalarIDF->Eber(pdrspqulChildren);
	SPQOS_RTL_ASSERT(eberResult == CScalar::EberTrue);

	pdrspqulChildren->Release();

	return SPQOS_OK;
}

static SPQOS_RESULT
EresUnittest_Eber_WhenBothInputsAreSameAndNotNull()
{
	SEberFixture fixture;
	CMemoryPool *mp = fixture.Pmp();

	ULongPtrArray *pdrspqulChildren = SPQOS_NEW(mp) ULongPtrArray(mp);
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberTrue));
	pdrspqulChildren->Append(SPQOS_NEW(mp) ULONG(CScalar::EberTrue));

	CScalarIsDistinctFrom *pScalarIDF = fixture.PScalarIDF();

	CScalar::EBoolEvalResult eberResult = pScalarIDF->Eber(pdrspqulChildren);
	SPQOS_RTL_ASSERT(eberResult == CScalar::EberFalse);

	pdrspqulChildren->Release();

	return SPQOS_OK;
}

SPQOS_RESULT
CScalarIsDistinctFromTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_Eber_WhenBothInputsAreNull),
		SPQOS_UNITTEST_FUNC(EresUnittest_Eber_WhenFirstInputIsUnknown),
		SPQOS_UNITTEST_FUNC(EresUnittest_Eber_WhenSecondInputIsUnknown),
		SPQOS_UNITTEST_FUNC(
			EresUnittest_Eber_WhenFirstInputDiffersFromSecondInput),
		SPQOS_UNITTEST_FUNC(EresUnittest_Eber_WhenBothInputsAreSameAndNotNull),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}
}  // namespace spqopt
