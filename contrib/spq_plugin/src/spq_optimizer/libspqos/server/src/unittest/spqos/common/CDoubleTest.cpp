//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDoubleTest.cpp
//
//	@doc:
//		Tests for the floating-point wrapper class.
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CDoubleTest.h"

#include "spqos/base.h"
#include "spqos/common/CDouble.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"


using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CDoubleTest::EresUnittest
//
//	@doc:
//		Driver for unittests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDoubleTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CDoubleTest::EresUnittest_Arithmetic),
		SPQOS_UNITTEST_FUNC(CDoubleTest::EresUnittest_Bool),
		SPQOS_UNITTEST_FUNC(CDoubleTest::EresUnittest_Convert),
		SPQOS_UNITTEST_FUNC(CDoubleTest::EresUnittest_Limits),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CDoubleTest::EresUnittest_Arithmetic
//
//	@doc:
//		Test arithmetic operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDoubleTest::EresUnittest_Arithmetic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CDouble fp1(2.5);
	CDouble fp2(3.5);

	CDouble fpAdd(fp1 + fp2);
	CDouble fpSubtract(fpAdd - fp2);
	CDouble fpMultiply(fp1 * fp2);
	CDouble fpDivide(fpMultiply / fp2);
	CDouble fpAbs(fp1.Absolute());
	CDouble fpFloor(fp1.Floor());
	CDouble fpCeil(fp1.Ceil());
	CDouble fpPow(fp1.Pow(fp2));
	CDouble fpLog2(fp2.Log2());

	SPQOS_RTL_ASSERT(fp1.Get() + fp2.Get() == fpAdd.Get());
	SPQOS_RTL_ASSERT(fpAdd.Get() - fp2.Get() == fpSubtract.Get());
	SPQOS_RTL_ASSERT(fp1.Get() == fpSubtract.Get());
	SPQOS_RTL_ASSERT(fp1.Get() * fp2.Get() == fpMultiply.Get());
	SPQOS_RTL_ASSERT(fpMultiply.Get() / fp2.Get() == fpDivide.Get());
	SPQOS_RTL_ASSERT(fp1.Get() == fpDivide.Get());
	SPQOS_RTL_ASSERT(fp1.Get() == fpAbs);
	SPQOS_RTL_ASSERT(1.0 == fpCeil - fpFloor);
	SPQOS_RTL_ASSERT(fpLog2 > 1.0 && fpLog2 < 2.0);

	CDouble fp3(10.0);
	fp3 = fp1 + fp2;

	CAutoTrace trace(mp);
	IOstream &os(trace.Os());

	os << "Arithmetic operations: " << std::endl
	   << fp1 << " + " << fp2 << " = " << fpAdd << std::endl
	   << fpAdd << " - " << fp2 << " = " << fpSubtract << std::endl
	   << fp1 << " * " << fp2 << " = " << fpMultiply << std::endl
	   << fpMultiply << " / " << fp2 << " = " << fpDivide << std::endl
	   << "Absolute(" << fp1 << ") = " << fpAbs << std::endl
	   << "Floor(" << fp1 << ") = " << fpCeil << std::endl
	   << "Ceil(" << fp1 << ") = " << fpCeil << std::endl
	   << "Pow(" << fp1 << "," << fp2 << ") = " << fpPow << std::endl
	   << "Log2(" << fp2 << ") = " << fpLog2 << std::endl
	   << fp1 << " + " << fp2 << " = " << fp3 << std::endl;

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CDoubleTest::EresUnittest_Bool
//
//	@doc:
//		Test comparison operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDoubleTest::EresUnittest_Bool()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CDouble fp1(2.5);
	CDouble fp2(3.5);
	CDouble fp3(3.5);

	SPQOS_ASSERT(fp1 < fp2);
	SPQOS_ASSERT(fp1 <= fp2);
	SPQOS_ASSERT(fp1 != fp2);
	SPQOS_ASSERT(fp2 > fp1);
	SPQOS_ASSERT(fp2 >= fp1);
	SPQOS_ASSERT(fp2 == fp3);
	SPQOS_ASSERT(fp2 >= fp3);
	SPQOS_ASSERT(fp2 <= fp3);

	CAutoTrace trace(mp);
	IOstream &os(trace.Os());

	os << "Boolean operations: " << std::endl
	   << fp1 << " < " << fp2 << " = " << (fp1 < fp2) << std::endl
	   << fp1 << " <= " << fp2 << " = " << (fp1 <= fp2) << std::endl
	   << fp1 << " != " << fp2 << " = " << (fp1 != fp2) << std::endl
	   << fp2 << " > " << fp1 << " = " << (fp2 > fp1) << std::endl
	   << fp2 << " >= " << fp1 << " = " << (fp2 >= fp1) << std::endl
	   << fp2 << " == " << fp3 << " = " << (fp2 == fp3) << std::endl
	   << fp2 << " >= " << fp3 << " = " << (fp2 >= fp2) << std::endl
	   << fp2 << " <= " << fp3 << " = " << (fp2 <= fp2) << std::endl;

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CDoubleTest::EresUnittest_Convert
//
//	@doc:
//		Test conversions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDoubleTest::EresUnittest_Convert()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CDouble fp(3.5);

	CAutoTrace trace(mp);
	IOstream &os(trace.Os());

	os << "Conversions:" << std::endl
	   << ULONG(10) << "u + " << fp << " = " << (ULONG(10) + fp) << std::endl
	   << ULLONG(10) << "ul - " << fp << " = " << (ULLONG(10) - fp) << std::endl
	   << INT(10) << " * " << fp << " = " << (INT(10) * fp) << std::endl
	   << LINT(10) << "l / " << fp << " = " << (LINT(10) / fp) << std::endl
	   << "-'10.0' = " << (-CDouble(clib::Strtod("10.0"))) << std::endl
	   << "Pow(" << ULONG(3) << ") = " << fp.Pow(ULONG(3)) << std::endl;

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CDoubleTest::EresUnittest_Limits
//
//	@doc:
//		Test underflow and overflow limits
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDoubleTest::EresUnittest_Limits()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CDouble fpZero(0);
	CDouble fpInf(1e10 / fpZero);

	CAutoTrace trace(mp);
	IOstream &os(trace.Os());

	SPQOS_ASSERT(fpZero == fpZero / fpInf);
	SPQOS_ASSERT(fpZero == fpZero / 2);
	SPQOS_ASSERT(fpInf == fpInf / fpZero);
	SPQOS_ASSERT(fpInf == fpInf * fpInf);
	SPQOS_ASSERT(fpInf == fpInf * 2);
	SPQOS_ASSERT(1.0 == fpInf * fpZero);
	SPQOS_ASSERT(1.0 == (fpInf * fpZero) * (fpInf * fpZero));
	SPQOS_ASSERT(1.0 == (fpInf * fpZero) / (fpInf * fpZero));

	os << "Limits:" << std::endl
	   << "zero = " << fpZero << std::endl
	   << "inf = " << fpInf << std::endl
	   << "zero / inf = " << (fpZero / fpInf) << std::endl
	   << "zero / 2 = " << (fpZero / 2) << std::endl
	   << "inf / zero = " << (fpInf / fpZero) << std::endl
	   << "inf * inf = " << (fpInf * fpInf) << std::endl
	   << "inf * 2 = " << (fpInf * fpInf) << std::endl
	   << "inf * zero = " << (fpInf * fpZero) << std::endl
	   << "(inf * zero) * (inf * zero) = "
	   << (fpInf * fpZero) * (fpInf * fpZero) << std::endl
	   << "(inf * zero) / (inf * zero) = "
	   << (fpInf * fpZero) / (fpInf * fpZero) << std::endl;

	return SPQOS_OK;
}

// EOF
