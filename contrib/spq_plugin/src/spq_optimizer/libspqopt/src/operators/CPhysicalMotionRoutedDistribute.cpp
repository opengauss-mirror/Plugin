//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalMotionRoutedDistribute.cpp
//
//	@doc:
//		Implementation of routed distribute motion operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalMotionRoutedDistribute.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::CPhysicalMotionRoutedDistribute
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalMotionRoutedDistribute::CPhysicalMotionRoutedDistribute(
	CMemoryPool *mp, CDistributionSpecRouted *pdsRouted)
	: CPhysicalMotion(mp), m_pdsRouted(pdsRouted), m_pcrsRequiredLocal(NULL)
{
	SPQOS_ASSERT(NULL != pdsRouted);

	m_pcrsRequiredLocal = SPQOS_NEW(mp) CColRefSet(mp);

	// include segment id column
	m_pcrsRequiredLocal->Include(m_pdsRouted->Pcr());
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::~CPhysicalMotionRoutedDistribute
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalMotionRoutedDistribute::~CPhysicalMotionRoutedDistribute()
{
	m_pdsRouted->Release();
	m_pcrsRequiredLocal->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::Matches
//
//	@doc:
//		Match operators
//
//---------------------------------------------------------------------------
BOOL
CPhysicalMotionRoutedDistribute::Matches(COperator *pop) const
{
	if (Eopid() != pop->Eopid())
	{
		return false;
	}

	CPhysicalMotionRoutedDistribute *popRoutedDistribute =
		CPhysicalMotionRoutedDistribute::PopConvert(pop);

	return m_pdsRouted->Matches(popRoutedDistribute->m_pdsRouted);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::PcrsRequired
//
//	@doc:
//		Compute required columns of the n-th child;
//
//---------------------------------------------------------------------------
CColRefSet *
CPhysicalMotionRoutedDistribute::PcrsRequired(CMemoryPool *mp,
											  CExpressionHandle &exprhdl,
											  CColRefSet *pcrsRequired,
											  ULONG child_index,
											  CDrvdPropArray *,	 // pdrspqdpCtxt
											  ULONG				 // ulOptReq
)
{
	SPQOS_ASSERT(0 == child_index);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, *m_pcrsRequiredLocal);
	pcrs->Union(pcrsRequired);

	CColRefSet *pcrsChildReqd =
		PcrsChildReqd(mp, exprhdl, pcrs, child_index, spqos::ulong_max);
	pcrs->Release();

	return pcrsChildReqd;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalMotionRoutedDistribute::FProvidesReqdCols(CExpressionHandle &exprhdl,
												   CColRefSet *pcrsRequired,
												   ULONG  // ulOptReq
) const
{
	return FUnaryProvidesReqdCols(exprhdl, pcrsRequired);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalMotionRoutedDistribute::EpetOrder(CExpressionHandle &,	 // exprhdl
										   const CEnfdOrder *	 // peo
) const
{
	return CEnfdProp::EpetRequired;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::PosRequired
//
//	@doc:
//		Compute required sort order of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalMotionRoutedDistribute::PosRequired(CMemoryPool *mp,
											 CExpressionHandle &,  // exprhdl
											 COrderSpec *,		   //posInput
											 ULONG
#ifdef SPQOS_DEBUG
												 child_index
#endif	// SPQOS_DEBUG
											 ,
											 CDrvdPropArray *,	// pdrspqdpCtxt
											 ULONG				// ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	return SPQOS_NEW(mp) COrderSpec(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalMotionRoutedDistribute::PosDerive(CMemoryPool *mp,
										   CExpressionHandle &	// exprhdl
) const
{
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalMotionRoutedDistribute::OsPrint(IOstream &os) const
{
	os << SzId() << " ";

	return m_pdsRouted->OsPrint(os);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalMotionRoutedDistribute::PopConvert
//
//	@doc:
//		Conversion function
//
//---------------------------------------------------------------------------
CPhysicalMotionRoutedDistribute *
CPhysicalMotionRoutedDistribute::PopConvert(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(EopPhysicalMotionRoutedDistribute == pop->Eopid());

	return dynamic_cast<CPhysicalMotionRoutedDistribute *>(pop);
}

// EOF
