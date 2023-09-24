//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CSerializablePlan.cpp
//
//	@doc:
//		Serializable plan object
//---------------------------------------------------------------------------

#include "spqopt/minidump/CSerializablePlan.h"

#include "spqos/base.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/task/CTask.h"

#include "naucrates/dxl/CDXLUtils.h"

using namespace spqos;
using namespace spqopt;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CSerializablePlan::CSerializablePlan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CSerializablePlan::CSerializablePlan(CMemoryPool *mp, const CDXLNode *pdxlnPlan,
									 ULLONG plan_id, ULLONG plan_space_size)
	: CSerializable(),
	  m_mp(mp),
	  m_plan_dxl_root(pdxlnPlan),
	  m_pstrPlan(NULL),
	  m_plan_id(plan_id),
	  m_plan_space_size(plan_space_size)
{
	SPQOS_ASSERT(NULL != pdxlnPlan);
}


//---------------------------------------------------------------------------
//	@function:
//		CSerializablePlan::~CSerializablePlan
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CSerializablePlan::~CSerializablePlan()
{
	SPQOS_DELETE(m_pstrPlan);
}

//---------------------------------------------------------------------------
//	@function:
//		CSerializablePlan::Serialize
//
//	@doc:
//		Serialize contents into provided stream
//
//---------------------------------------------------------------------------
void
CSerializablePlan::Serialize(COstream &oos)
{
	CDXLUtils::SerializePlan(m_mp, oos, m_plan_dxl_root, m_plan_id,
							 m_plan_space_size, false /*fSerializeHeaders*/,
							 false /*indentation*/
	);
}

// EOF
