//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalMotionRandom.cpp
//
//	@doc:
//		Implementation of random motion operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalMotionRandom.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::CPhysicalMotionRandom
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalMotionRandom::CPhysicalMotionRandom(CMemoryPool *mp,
											 CDistributionSpecRandom *pdsRandom)
	: CPhysicalMotion(mp), m_pdsRandom(pdsRandom)
{
	SPQOS_ASSERT(NULL != pdsRandom);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::~CPhysicalMotionRandom
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalMotionRandom::~CPhysicalMotionRandom()
{
	m_pdsRandom->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::Matches
//
//	@doc:
//		Match operators
//
//---------------------------------------------------------------------------
BOOL
CPhysicalMotionRandom::Matches(COperator *pop) const
{
	return Eopid() == pop->Eopid();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::PcrsRequired
//
//	@doc:
//		Compute required columns of the n-th child;
//
//---------------------------------------------------------------------------
CColRefSet *
CPhysicalMotionRandom::PcrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									CColRefSet *pcrsRequired, ULONG child_index,
									CDrvdPropArray *,  // pdrspqdpCtxt
									ULONG			   // ulOptReq
)
{
	SPQOS_ASSERT(0 == child_index);

	return PcrsChildReqd(mp, exprhdl, pcrsRequired, child_index,
						 spqos::ulong_max);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalMotionRandom::FProvidesReqdCols(CExpressionHandle &exprhdl,
										 CColRefSet *pcrsRequired,
										 ULONG	// ulOptReq
) const
{
	return FUnaryProvidesReqdCols(exprhdl, pcrsRequired);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalMotionRandom::EpetOrder(CExpressionHandle &,  // exprhdl
								 const CEnfdOrder *
#ifdef SPQOS_DEBUG
									 peo
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	return CEnfdProp::EpetRequired;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::PosRequired
//
//	@doc:
//		Compute required sort order of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalMotionRandom::PosRequired(CMemoryPool *mp,
								   CExpressionHandle &,	 //exprhdl,
								   COrderSpec *,		 //posInput,
								   ULONG
#ifdef SPQOS_DEBUG
									   child_index
#endif	// SPQOS_DEBUG
								   ,
								   CDrvdPropArray *,  // pdrspqdpCtxt
								   ULONG			  // ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	return SPQOS_NEW(mp) COrderSpec(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalMotionRandom::PosDerive(CMemoryPool *mp,
								 CExpressionHandle &  // exprhdl
) const
{
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalMotionRandom::OsPrint(IOstream &os) const
{
	os << SzId();

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRandom::PopConvert
//
//	@doc:
//		Conversion function
//
//---------------------------------------------------------------------------
CPhysicalMotionRandom *
CPhysicalMotionRandom::PopConvert(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(EopPhysicalMotionRandom == pop->Eopid());

	return dynamic_cast<CPhysicalMotionRandom *>(pop);
}

// EOF
