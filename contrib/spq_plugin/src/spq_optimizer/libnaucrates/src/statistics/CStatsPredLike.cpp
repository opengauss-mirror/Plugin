//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CStatsPredLike.cpp
//
//	@doc:
//		Implementation of statistics LIKE filter
//---------------------------------------------------------------------------

#include "naucrates/statistics/CStatsPredLike.h"

#include "spqopt/operators/CExpression.h"

using namespace spqnaucrates;
using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CStatsPredLike::CStatisticsFilterLike
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CStatsPredLike::CStatsPredLike(ULONG colid, CExpression *expr_left,
							   CExpression *expr_right,
							   CDouble default_scale_factor)
	: CStatsPred(colid),
	  m_expr_left(expr_left),
	  m_expr_right(expr_right),
	  m_default_scale_factor(default_scale_factor)
{
	SPQOS_ASSERT(spqos::ulong_max != colid);
	SPQOS_ASSERT(NULL != expr_left);
	SPQOS_ASSERT(NULL != expr_right);
	SPQOS_ASSERT(0 < default_scale_factor);
}

//---------------------------------------------------------------------------
//	@function:
//		CStatsPredLike::~CStatisticsFilterLike
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CStatsPredLike::~CStatsPredLike()
{
	m_expr_left->Release();
	m_expr_right->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CStatsPredLike::GetColId
//
//	@doc:
//		Return the column identifier on which the predicates are on
//
//---------------------------------------------------------------------------
ULONG
CStatsPredLike::GetColId() const
{
	return m_colid;
}

//---------------------------------------------------------------------------
//	@function:
//		CStatsPredLike::DefaultScaleFactor
//
//	@doc:
//		Return the default like scale factor
//
//---------------------------------------------------------------------------
CDouble
CStatsPredLike::DefaultScaleFactor() const
{
	return m_default_scale_factor;
}

// EOF
