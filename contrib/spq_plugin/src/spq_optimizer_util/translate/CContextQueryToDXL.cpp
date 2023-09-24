//---------------------------------------------------------------------------
//  Greenplum Database
//	Copyright (C) 2018-Present Pivotal Software, Inc.
//
//	@filename:
//		CContextQueryToDXL.cpp
//
//	@doc:
//		Implementation of the methods used to hold information about
//		the whole query, when translate a query into DXL tree. All
//		translator methods allocate memory in the provided memory pool,
//		and the caller is responsible for freeing it
//
//---------------------------------------------------------------------------
#include "spq_optimizer_util/translate/CContextQueryToDXL.h"

#include "spq_optimizer_util/translate/CTranslatorUtils.h"
#include "naucrates/dxl/CIdGenerator.h"

using namespace spqdxl;

CContextQueryToDXL::CContextQueryToDXL(CMemoryPool *mp)
	: m_mp(mp),
	  m_has_distributed_tables(false),
	  m_distribution_hashops(DistrHashOpsNotDeterminedYet)
{
	// map that stores spqdb att to optimizer col mapping
	m_colid_counter = SPQOS_NEW(mp) CIdGenerator(SPQDXL_COL_ID_START);
	m_cte_id_counter = SPQOS_NEW(mp) CIdGenerator(SPQDXL_CTE_ID_START);
}

CContextQueryToDXL::~CContextQueryToDXL()
{
	SPQOS_DELETE(m_colid_counter);
	SPQOS_DELETE(m_cte_id_counter);
}
