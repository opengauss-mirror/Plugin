//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCTEListEntry.cpp
//
//	@doc:
//		Implementation of the class representing the list of common table
//		expression defined at a query level
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "postgres.h"

#include "nodes/parsenodes.h"
#include "spqos/base.h"

#include "spq_optimizer_util/spq_wrappers.h"
#include "spq_optimizer_util/translate/CCTEListEntry.h"
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CCTEListEntry::CCTEListEntry
//
//	@doc:
//		Ctor: single CTE
//
//---------------------------------------------------------------------------
CCTEListEntry::CCTEListEntry(CMemoryPool *mp, ULONG query_level,
							 CommonTableExpr *cte, CDXLNode *cte_producer)
	: m_query_level(query_level), m_cte_info(NULL)
{
	SPQOS_ASSERT(NULL != cte && NULL != cte_producer);

	m_cte_info = SPQOS_NEW(mp) HMSzCTEInfo(mp);
	Query *cte_query = (Query *) cte->ctequery;

#ifdef SPQOS_DEBUG
	BOOL result =
#endif
		m_cte_info->Insert(
			cte->ctename,
			SPQOS_NEW(mp) SCTEProducerInfo(cte_producer, cte_query->targetList));

	SPQOS_ASSERT(result);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEListEntry::CCTEListEntry
//
//	@doc:
//		Ctor: multiple CTEs
//
//---------------------------------------------------------------------------
CCTEListEntry::CCTEListEntry(CMemoryPool *mp, ULONG query_level, List *cte_list,
							 CDXLNodeArray *cte_dxl_arr)
	: m_query_level(query_level), m_cte_info(NULL)
{
	SPQOS_ASSERT(NULL != cte_dxl_arr);
	SPQOS_ASSERT(cte_dxl_arr->Size() == spqdb::ListLength(cte_list));

	m_cte_info = SPQOS_NEW(mp) HMSzCTEInfo(mp);
	const ULONG num_cte = cte_dxl_arr->Size();

	for (ULONG ul = 0; ul < num_cte; ul++)
	{
		CDXLNode *cte_producer = (*cte_dxl_arr)[ul];
		CommonTableExpr *cte = (CommonTableExpr *) spqdb::ListNth(cte_list, ul);

		Query *cte_query = (Query *) cte->ctequery;

#ifdef SPQOS_DEBUG
		BOOL result =
#endif
			m_cte_info->Insert(cte->ctename,
							   SPQOS_NEW(mp) SCTEProducerInfo(
								   cte_producer, cte_query->targetList));

		SPQOS_ASSERT(result);
		SPQOS_ASSERT(NULL != m_cte_info->Find(cte->ctename));
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEListEntry::GetCTEProducer
//
//	@doc:
//		Return the query of the CTE referenced in the range table entry
//
//---------------------------------------------------------------------------
const CDXLNode *
CCTEListEntry::GetCTEProducer(const CHAR *cte_str) const
{
	SCTEProducerInfo *cte_info = m_cte_info->Find(cte_str);
	if (NULL == cte_info)
	{
		return NULL;
	}

	return cte_info->m_cte_producer;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEListEntry::GetCTEProducerTargetList
//
//	@doc:
//		Return the target list of the CTE referenced in the range table entry
//
//---------------------------------------------------------------------------
List *
CCTEListEntry::GetCTEProducerTargetList(const CHAR *cte_str) const
{
	SCTEProducerInfo *cte_info = m_cte_info->Find(cte_str);
	if (NULL == cte_info)
	{
		return NULL;
	}

	return cte_info->m_target_list;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEListEntry::AddCTEProducer
//
//	@doc:
//		Add a new CTE producer to this query level
//
//---------------------------------------------------------------------------
void
CCTEListEntry::AddCTEProducer(CMemoryPool *mp, CommonTableExpr *cte,
							  const CDXLNode *cte_producer)
{
	SPQOS_ASSERT(NULL == m_cte_info->Find(cte->ctename) &&
				"CTE entry already exists");
	Query *cte_query = (Query *) cte->ctequery;

#ifdef SPQOS_DEBUG
	BOOL result =
#endif
		m_cte_info->Insert(
			cte->ctename,
			SPQOS_NEW(mp) SCTEProducerInfo(cte_producer, cte_query->targetList));

	SPQOS_ASSERT(result);
}

// EOF
